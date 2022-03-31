//=========== system headers ===========
#include <Arduino.h>

//=========== library headers ===========
#include <Audio.h>


namespace defaults {

/**
 * @brief Enumeration of waveforms that can be used for the signal generation.
 * The values are copied from the Teensy Audio Library.
 *
 */
enum class Waveform : short {
  kSine = 0,
  kSawtooth = 1,
  kSquare = 2,
  kTriangle = 3,
  kArbitrary = 4,
  kPulse = 5,
  kSawtoothReverse = 6,
  kSampleHold = 7,
  kTriangleVariable = 8,
  kBandlimitSawtooth = 9,
  kBandlimitSawtoothReverse = 10,
  kBandlimitSquare = 11,
  kBandlimitPulse = 12
};

//=========== signal generator ===========
static constexpr uint16_t kNumberOfBins = 50;
static constexpr uint32_t kSignalDurationUs = 10000;
static constexpr short kSignalWaveform = static_cast<short>(Waveform::kSine);
static constexpr float kSignalFreqencyHz = 100.f;
static constexpr float kSignalAmpPos = 0.56f;
static constexpr float kSignalAmpNeg = 0.43f;

//=========== sensor ===========
static constexpr float kFilterWeight = 0.2;
static constexpr uint8_t kSensorResolution = 10;
static constexpr uint32_t kSensorMaxValue = (1U << kSensorResolution) - 1;
static constexpr uint32_t kSensorMinValue = 0;
static constexpr uint8_t kAnalogSensingPin = A0;

//=========== serial ===========
static constexpr int kBaudRate = 115200;

//=========== servo ===========
static constexpr uint32_t kServoDelayMs = 20;
static constexpr uint8_t kServoInputPin = 17;
}  // namespace defaults

namespace lut {
static constexpr uint8_t kSize = 181;
static constexpr uint8_t kMaxIndex = kSize - 1;

/**
 * @brief Lookup table for the servo angle to number of bins conversion.
 * The values are created by the LUTgenerator (Processing) with the following
 * parameters: [ binMin: 10 binMax: 100 binLevels: 12]
 */
static constexpr uint16_t kNumberOfBins[] = {
    10,  10,  10,  10,  10,  10,  10,  10,  10,  10,  10,  10,  10,  10,
    10,  18,  18,  18,  18,  18,  18,  18,  18,  18,  18,  18,  18,  18,
    18,  18,  26,  26,  26,  26,  26,  26,  26,  26,  26,  26,  26,  26,
    26,  26,  26,  35,  35,  35,  35,  35,  35,  35,  35,  35,  35,  35,
    35,  35,  35,  35,  43,  43,  43,  43,  43,  43,  43,  43,  43,  43,
    43,  43,  43,  43,  43,  51,  51,  51,  51,  51,  51,  51,  51,  51,
    51,  51,  51,  51,  51,  51,  59,  59,  59,  59,  59,  59,  59,  59,
    59,  59,  59,  59,  59,  59,  59,  67,  67,  67,  67,  67,  67,  67,
    67,  67,  67,  67,  67,  67,  67,  67,  75,  75,  75,  75,  75,  75,
    75,  75,  75,  75,  75,  75,  75,  75,  75,  84,  84,  84,  84,  84,
    84,  84,  84,  84,  84,  84,  84,  84,  84,  84,  92,  92,  92,  92,
    92,  92,  92,  92,  92,  92,  92,  92,  92,  92,  92,  100, 100, 100,
    100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100};

/**
 * @brief Lookup table for the servo angle to frequency conversion.
 * The values are created by the LUTgenerator (Processing) with the following
 * parameters: [ freqMin: 10.0 freqMax: 300.0 freqLevels: 8]
 */
static constexpr float kFrequencies[] = {
    10.0,  10.0,  51.0,  51.0,  93.0,  93.0,  134.0, 134.0, 176.0, 176.0, 217.0,
    217.0, 259.0, 259.0, 300.0, 10.0,  51.0,  51.0,  93.0,  93.0,  134.0, 134.0,
    176.0, 176.0, 217.0, 217.0, 259.0, 259.0, 300.0, 300.0, 10.0,  10.0,  51.0,
    51.0,  93.0,  93.0,  134.0, 134.0, 176.0, 176.0, 217.0, 217.0, 259.0, 259.0,
    300.0, 10.0,  51.0,  51.0,  93.0,  93.0,  134.0, 134.0, 176.0, 176.0, 217.0,
    217.0, 259.0, 259.0, 300.0, 300.0, 10.0,  10.0,  51.0,  51.0,  93.0,  93.0,
    134.0, 134.0, 176.0, 176.0, 217.0, 217.0, 259.0, 259.0, 300.0, 10.0,  51.0,
    51.0,  93.0,  93.0,  134.0, 134.0, 176.0, 176.0, 217.0, 217.0, 259.0, 259.0,
    300.0, 300.0, 10.0,  10.0,  51.0,  51.0,  93.0,  93.0,  134.0, 134.0, 176.0,
    176.0, 217.0, 217.0, 259.0, 259.0, 300.0, 10.0,  51.0,  51.0,  93.0,  93.0,
    134.0, 134.0, 176.0, 176.0, 217.0, 217.0, 259.0, 259.0, 300.0, 300.0, 10.0,
    10.0,  51.0,  51.0,  93.0,  93.0,  134.0, 134.0, 176.0, 176.0, 217.0, 217.0,
    259.0, 259.0, 300.0, 10.0,  51.0,  51.0,  93.0,  93.0,  134.0, 134.0, 176.0,
    176.0, 217.0, 217.0, 259.0, 259.0, 300.0, 300.0, 10.0,  10.0,  51.0,  51.0,
    93.0,  93.0,  134.0, 134.0, 176.0, 176.0, 217.0, 217.0, 259.0, 259.0, 300.0,
    10.0,  51.0,  51.0,  93.0,  93.0,  134.0, 134.0, 176.0, 176.0, 217.0, 217.0,
    259.0, 259.0, 300.0, 300.0, 10.0};
}  // namespace lut

namespace {

/**
 * @brief a struct to hold the settings of the sensor.
 *
 */
typedef struct {
  float filter_weight = defaults::kFilterWeight;
  uint8_t resolution = defaults::kSensorResolution;
  uint32_t max_value = defaults::kSensorMaxValue;
  uint32_t min_value = defaults::kSensorMinValue;
} SensorSettings;

/**
 * @brief a struct to hold the settings for the signal generator.
 *
 */
typedef struct {
  uint16_t number_of_bins = defaults::kNumberOfBins;
  uint32_t duration_us = defaults::kSignalDurationUs;
  short waveform = defaults::kSignalWaveform;
  float frequency_hz = defaults::kSignalFreqencyHz;
  float amp_pos = defaults::kSignalAmpPos;
  float amp_neg = defaults::kSignalAmpNeg;
} SignalGeneratorSettings;

//=========== settings instances ===========
// These instances are used to access the settings in the main code.
SensorSettings sensor_settings;
SignalGeneratorSettings signal_generator_settings;

//=========== audio variables ===========
AudioSynthWaveform signal;
AudioOutputPT8211 to_haptuator;
AudioConnection patchCord1(signal, 0, to_haptuator, 0);
AudioConnection patchCord2(signal, 0, to_haptuator, 1);

//=========== sensor variables ===========
float filtered_sensor_value = 0.f;

//=========== control flow variables ===========
elapsedMicros pulse_time_us = 0;
bool is_vibrating = false;
uint16_t last_bin_id = 0;
uint16_t new_pulse_id = 0;
uint16_t current_pulse_id = 0;

//=========== servo variables ===========
static constexpr int kMinServoPulseLength = 544;
static constexpr int kMaxServoPulseLength = 2400;
static constexpr int kMinServoAngle = 0;
static constexpr int kMaxServoAngle = 180;
elapsedMillis servo_timer_ms = 0;
volatile uint32_t servo_pulse_length = 0;
volatile bool is_new_servo_pulse = false;
uint8_t servo_angle = 0;
uint8_t last_servo_angle = 255;

//=========== helper functions ===========
// These functions were extracted to simplify the control flow and will be
// inlined by the compiler.
inline void SetupAudio() __attribute__((always_inline));
inline void StartPulse() __attribute__((always_inline));
inline void StopPulse() __attribute__((always_inline));
inline void HandleServoPulse() __attribute__((always_inline));
void ServoPinChangingEdge();
void UpdateSettingsFromLUTs(uint8_t index);

/**
 * @brief set up the audio system
 *
 */
void SetupAudio() {
  AudioMemory(20);
  delay(50);  // time for DAC voltage stable
  signal.begin(signal_generator_settings.waveform);
  signal.frequency(signal_generator_settings.frequency_hz);
}

/**
 * @brief start a pulse by setting the amplitude of the signal to a predefined
 * value
 *
 */
void StartPulse() {
  pulse_time_us = 0;
  signal.amplitude(signal_generator_settings.amp_pos);
  signal.begin(signal_generator_settings.waveform);
  is_vibrating = true;
}

/**
 * @brief stop the pulse by setting the amplitude of the signal to zero
 *
 */
void StopPulse() {
  signal.amplitude(0.f);
  is_vibrating = false;
}

void ServoPinChangingEdge() {
  if (digitalReadFast(defaults::kServoInputPin)) {
    is_new_servo_pulse = false;
    servo_pulse_length = micros();
  } else {
    servo_pulse_length = micros() - servo_pulse_length;
    is_new_servo_pulse = true;
  }
}

void HandleServoPulse() {
  if (servo_pulse_length >= kMinServoPulseLength &&
      servo_pulse_length <= kMaxServoPulseLength) {
    servo_angle = (uint8_t)map(servo_pulse_length, kMinServoPulseLength,
                    kMaxServoPulseLength, kMinServoAngle, kMaxServoAngle);
    if (servo_angle != last_servo_angle) {
      UpdateSettingsFromLUTs(servo_angle);
      last_servo_angle = servo_angle;
    }
  }
}

/**
 * @brief selects a set of parameters for the signal generator from a set of
 * lookup tables. The lookup tables are defined in the lut namespace. To follow
 * the servo logic, we define each lookup table as an array values in the range
 * [0, 180].
 *
 */
void UpdateSettingsFromLUTs(uint8_t index) {
  if (index > lut::kMaxIndex) {
    index = lut::kMaxIndex;
  }
  signal_generator_settings.number_of_bins = lut::kNumberOfBins[index];
  signal_generator_settings.frequency_hz = lut::kFrequencies[index];
}

}  // namespace


void setup() {
  // initialize the serial communication
  while (!Serial && millis() < 5000)
    ;
  Serial.begin(defaults::kBaudRate);

  // initialize the audio processing
  SetupAudio();

  // initialize the sensor
  pinMode(defaults::kAnalogSensingPin, INPUT);
  analogReadRes(sensor_settings.resolution);

  // initialize the servo
  pinMode(defaults::kServoInputPin, INPUT_PULLDOWN);
  attachInterrupt(defaults::kServoInputPin, ServoPinChangingEdge, CHANGE);
}


void loop() {
  if (is_new_servo_pulse && servo_timer_ms > defaults::kServoDelayMs) {
    HandleServoPulse();
    servo_timer_ms = 0;
  }
  
  // read the sensor value and filter it
  auto sensor_value = analogRead(defaults::kAnalogSensingPin);
  filtered_sensor_value =
      (1.f - sensor_settings.filter_weight) * filtered_sensor_value +
      sensor_settings.filter_weight * sensor_value;

  // calculate the bin id depending on the filtered sensor value
  // (currently linear mapping)
  uint16_t mapped_bin_id = map(filtered_sensor_value, sensor_settings.min_value,
                               sensor_settings.max_value, 0,
                               signal_generator_settings.number_of_bins);

  if (mapped_bin_id != last_bin_id) {
    //! bin CHANGED

    // ******* #1 Assign a pulse number *************************
    if (mapped_bin_id > last_bin_id) {
      new_pulse_id = mapped_bin_id;
    } else {
      new_pulse_id = mapped_bin_id;
    }

    // ******* #2 Check if Pulse is already vibrating *************************
    // if its not vibrating, start a new pulse with a new ID
    if (!is_vibrating) {
      // current_pulse_id = new_pulse_id;
    }

    // ******* #3 Check if we have a new Pulse ID *************************
    if (new_pulse_id == current_pulse_id) {
      // don't do anything if its already vibrating
      // with an existing pulse
      // don't retrigger the same pulse!
      //(this is probably not needed)
    } else {
      //! currently same pulses cannot retrigger at all
      //! this is maybe too strict
      current_pulse_id = new_pulse_id;
      StartPulse();
    }

    last_bin_id = mapped_bin_id;
  }

  if (is_vibrating) {
    if (pulse_time_us >= signal_generator_settings.duration_us) {
      StopPulse();
    }
  }
}
