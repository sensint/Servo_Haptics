// Updated code of binning algorithm to adjust for delays, debounce and latency
// caused by high filter weights. Setting the max value based on the average of
// the first 10 measurements and then mapping it to the full range (or using
// that as the max and min in the mapping function).

//=========== include libraries ===========
#include <Arduino.h>
#include <Audio.h>

//=========== Audio Variables ===========
AudioSynthWaveform signal;
AudioOutputPT8211 toHaptuator;
AudioConnection patchCord1(signal, 0, toHaptuator, 0);
AudioConnection patchCord2(signal, 0, toHaptuator, 1);

//=========== Other Variables ===========
#define MESSAGESIZE 4
//{#bins, freq, amp-pos, amp-neg}
float receivedMessage[MESSAGESIZE] = {50, 100, 0.56, 0.43};
unsigned long vibrationDuration = 10;
unsigned long vibrationStartTime;

//=========== Sensor Variables ===========
#define sensingPin A0
float filtered_val = 0.0;
float filter_weight = 0.2;

const uint8_t sensorResolution = 10;  // 10 bit resolution
const int sensorMaxValue = (1 << sensorResolution) - 1;
const int sensorMinValue = 0;
int led = 13;
int count = 0;

//=========== Descriptors ===========
bool slope_is_positive = true;
int direction = 1;
int lastBinNumber = 0;
bool isVibrating = false;  // Checks if the haptuator is vibrating
bool newTrigger = true;    // New trigger to initiate the one bin
int newPulseID = 0;        // ID of new pulse
int currentPulseID = 0;    // ID of ongoing pulse
// its allways the smaller of the two option,
// so when going from 4 to 5, its 5, when going from 7 to 6 its 6

// do we need a current bin-number?
//======================================

void setup() {
  AudioMemory(20);
  delay(50);  // time for DAC voltage stable
  // Serial.begin(115200);
  pinMode(sensingPin, INPUT);
  analogReadRes(sensorResolution);  // Uncomment this for button press.

  signal.begin(WAVEFORM_SINE);
  signal.frequency(receivedMessage[1]);
  pinMode(led, OUTPUT);  // Used for Debugging and debounce condition
}

void loop() {
  auto rheo_read = analogRead(sensingPin);
  filtered_val =
      (1.0 - filter_weight) * filtered_val + filter_weight * rheo_read;
  // Function for the type of mapping. (Currently used is Linear)
  int currentBinNumber =
      map(filtered_val, sensorMinValue, sensorMaxValue, 0, receivedMessage[0]);

  //=========== Logic ===========
  // Trigger if pointer moves into a new bin.
  if (currentBinNumber != lastBinNumber) {
    // ******* #1 Assign a pulse number *************************
    // check for the pulse ID
    if (currentBinNumber > lastBinNumber) {
      newPulseID = currentBinNumber;
    } else {
      newPulseID = lastBinNumber;
    }

    // ******* #2 Check if Pulse is already vibrating *************************
    // if its not vibrating, start a new pulse with a new ID
    if (!isVibrating) {
      // currentPulseID = newPulseID;
    }

    // ******* #3 Check if we have a new Pulse ID *************************
    if (newPulseID == currentPulseID) {
      // don't do anything if its already vibrating
      // with an existing pulse
      // don't retrigger the same pulse!
      //(this is probably not needed)
    } else {
      //! currently same pulses cannot retrigger at all
      //! this is maybe too strict
      currentPulseID = newPulseID;
      vibrationStartTime = millis();
      signal.amplitude(
          receivedMessage[2]);  //<-- maybe give these explicit variables
      isVibrating = true;
    }
    lastBinNumber = currentBinNumber;
  }

  if (isVibrating) {
    // if the duration is over set volume to zero
    if (millis() >= vibrationStartTime + vibrationDuration) {
      signal.amplitude(0.f);
      isVibrating = false;
    }
  }
}
