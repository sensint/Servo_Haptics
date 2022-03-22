#ifndef SENSINT_SETTINGS_H
#define SENSINT_SETTINGS_H

/**
 * @brief This file provides the settings for the senSInt project.
 * These are the parameters that could change during the operation of the system
 * - either in your own code or via the serial interface. An example of such a
 * parameter is implemented in the function
 * @ref UpdateSettingsFromSerialInput.
 *
 * If you want to set global parameters (configurations) that should not change
 * (constants), add them to the file config.h.
 */

namespace sensint {
namespace settings {

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

namespace defaults {
//=========== sensor ===========
static constexpr float kFilterWeight = 0.2;
static constexpr uint8_t kSensorResolution = 10;
static constexpr uint32_t kSensorMaxValue = (1U << kSensorResolution) - 1;
static constexpr uint32_t kSensorMinValue = 0;

//=========== signal generator ===========
static constexpr uint16_t kNumberOfBins = 50;
static constexpr uint32_t kSignalDurationMs = 10;
static constexpr short kSignalWaveform = static_cast<short>(Waveform::kSine);
static constexpr float kSignalFreqencyHz = 100.f;
static constexpr float kSignalAmpPos = 0.56f;
static constexpr float kSignalAmpNeg = 0.43f;
}  // namespace defaults

typedef struct {
  float filter_weight = defaults::kFilterWeight;
  uint8_t resolution = defaults::kSensorResolution;
  uint32_t max_value = defaults::kSensorMaxValue;
  uint32_t min_value = defaults::kSensorMinValue;
} SensorSettings;

typedef struct {
  uint16_t number_of_bins = defaults::kNumberOfBins;
  uint32_t duration_ms = defaults::kSignalDurationMs;
  short waveform = defaults::kSignalWaveform;
  float frequency_hz = defaults::kSignalFreqencyHz;
  float amp_pos = defaults::kSignalAmpPos;
  float amp_neg = defaults::kSignalAmpNeg;
} SignalGeneratorSettings;

//=========== settings instances ===========
// These instances are used to access the settings in the main code.
static SensorSettings sensor_settings;
static SignalGeneratorSettings signal_generator_settings;

/**
 * @brief reads the serial input and updates the settings accordingly
 *
 */
static void UpdateSettingsFromSerialInput() {
  if (Serial.available() == 0) {
    return;
  }
  auto input = Serial.readStringUntil('\n');
  input.toLowerCase();
  input.trim();
  switch (input.charAt(0)) {
    case 'a': {
      sensor_settings.filter_weight = input.substring(1).toFloat();
      break;
    }
    case 'b': {
      sensor_settings.resolution = input.substring(1).toInt();
      break;
    }
    case 'c': {
      signal_generator_settings.number_of_bins = input.substring(1).toInt();
      break;
    }
    case 'd': {
      signal_generator_settings.duration_ms = input.substring(1).toInt();
      break;
    }
    case 'e': {
      short val = input.substring(1).toInt();
      signal_generator_settings.waveform = (val < 0 || val > 12) ? 0 : val;
      break;
    }
    case 'f': {
      signal_generator_settings.frequency_hz = input.substring(1).toFloat();
      break;
    }
    case 'g': {
      signal_generator_settings.amp_pos = input.substring(1).toFloat();
      break;
    }
    case 'h': {
      signal_generator_settings.amp_neg = input.substring(1).toFloat();
      break;
    }
    default:
      return;
  }
}

}  // namespace settings
}  // namespace sensint

#endif  // SENSINT_SETTINGS_H
