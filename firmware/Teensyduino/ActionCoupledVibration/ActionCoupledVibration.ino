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
}  // namespace defaults

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

//=========== helper functions ===========
// These functions were extracted to simplify the control flow and will be
// inlined by the compiler.
inline void StartPulse() __attribute__((always_inline));
inline void StopPulse() __attribute__((always_inline));

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
}  // namespace

void setup() {
  // initialize the serial communication
  while (!Serial && millis() < 5000)
    ;
  Serial.begin(defaults::kBaudRate);

  // initialize the audio processing
  AudioMemory(20);
  delay(50);  // time for DAC voltage stable
  signal.begin(signal_generator_settings.waveform);
  signal.frequency(signal_generator_settings.frequency_hz);

  // initialize the sensor
  pinMode(defaults::kAnalogSensingPin, INPUT);
  analogReadRes(sensor_settings.resolution);
}

void loop() {
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
