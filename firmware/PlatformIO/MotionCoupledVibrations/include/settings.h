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

//=========== Lookup Tables ===========
namespace lut {
static constexpr uint8_t kSize = 181;
static constexpr uint8_t kMaxIndex = kSize - 1;

// TODO: replace with lookup table from the Processing generator
static constexpr uint16_t kNumberOfBins[] = {
    0,   1,   2,   3,   4,   5,   6,   7,   8,   9,   10,  11,  12,  13,
    14,  15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,
    28,  29,  30,  31,  32,  33,  34,  35,  36,  37,  38,  39,  40,  41,
    42,  43,  44,  45,  46,  47,  48,  49,  50,  51,  52,  53,  54,  55,
    56,  57,  58,  59,  60,  61,  62,  63,  64,  65,  66,  67,  68,  69,
    70,  71,  72,  73,  74,  75,  76,  77,  78,  79,  80,  81,  82,  83,
    84,  85,  86,  87,  88,  89,  90,  91,  92,  93,  94,  95,  96,  97,
    98,  99,  100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111,
    112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125,
    126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139,
    140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153,
    154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167,
    168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179, 180};

// TODO: replace with lookup table from the Processing generator
static constexpr float kFrequencies[] = {
    0.0f,   1.0f,   2.0f,   3.0f,   4.0f,   5.0f,   6.0f,   7.0f,   8.0f,
    9.0f,   10.0f,  11.0f,  12.0f,  13.0f,  14.0f,  15.0f,  16.0f,  17.0f,
    18.0f,  19.0f,  20.0f,  21.0f,  22.0f,  23.0f,  24.0f,  25.0f,  26.0f,
    27.0f,  28.0f,  29.0f,  30.0f,  31.0f,  32.0f,  33.0f,  34.0f,  35.0f,
    36.0f,  37.0f,  38.0f,  39.0f,  40.0f,  41.0f,  42.0f,  43.0f,  44.0f,
    45.0f,  46.0f,  47.0f,  48.0f,  49.0f,  50.0f,  51.0f,  52.0f,  53.0f,
    54.0f,  55.0f,  56.0f,  57.0f,  58.0f,  59.0f,  60.0f,  61.0f,  62.0f,
    63.0f,  64.0f,  65.0f,  66.0f,  67.0f,  68.0f,  69.0f,  70.0f,  71.0f,
    72.0f,  73.0f,  74.0f,  75.0f,  76.0f,  77.0f,  78.0f,  79.0f,  80.0f,
    81.0f,  82.0f,  83.0f,  84.0f,  85.0f,  86.0f,  87.0f,  88.0f,  89.0f,
    90.0f,  91.0f,  92.0f,  93.0f,  94.0f,  95.0f,  96.0f,  97.0f,  98.0f,
    99.0f,  100.0f, 101.0f, 102.0f, 103.0f, 104.0f, 105.0f, 106.0f, 107.0f,
    108.0f, 109.0f, 110.0f, 111.0f, 112.0f, 113.0f, 114.0f, 115.0f, 116.0f,
    117.0f, 118.0f, 119.0f, 120.0f, 121.0f, 122.0f, 123.0f, 124.0f, 125.0f,
    126.0f, 127.0f, 128.0f, 129.0f, 130.0f, 131.0f, 132.0f, 133.0f, 134.0f,
    135.0f, 136.0f, 137.0f, 138.0f, 139.0f, 140.0f, 141.0f, 142.0f, 143.0f,
    144.0f, 145.0f, 146.0f, 147.0f, 148.0f, 149.0f, 150.0f, 151.0f, 152.0f,
    153.0f, 154.0f, 155.0f, 156.0f, 157.0f, 158.0f, 159.0f, 160.0f, 161.0f,
    162.0f, 163.0f, 164.0f, 165.0f, 166.0f, 167.0f, 168.0f, 169.0f, 170.0f,
    171.0f, 172.0f, 173.0f, 174.0f, 175.0f, 176.0f, 177.0f, 178.0f, 179.0f,
    180.0f};
}  // namespace lut

namespace defaults {
//=========== sensor ===========
static constexpr float kFilterWeight = 0.2;
static constexpr uint8_t kSensorResolution = 10;
static constexpr uint32_t kSensorMaxValue = (1U << kSensorResolution) - 1;
static constexpr uint32_t kSensorMinValue = 0;

//=========== signal generator ===========
// TODO: reset to default values from experimental setup
static constexpr uint16_t kNumberOfBins = 50;
static constexpr uint32_t kSignalDurationUs = 10000;
static constexpr short kSignalWaveform = static_cast<short>(
    Waveform::kSquare);  // static_cast<short>(Waveform::kSine);
static constexpr float kSignalFreqencyHz = 100.f;
static constexpr float kSignalAmpPos = .2f;  // 0.56f;
static constexpr float kSignalAmpNeg = 0.43f;
}  // namespace defaults

typedef struct {
  float filter_weight = defaults::kFilterWeight;
  uint8_t resolution = defaults::kSensorResolution;
  uint32_t max_value = defaults::kSensorMaxValue;
  uint32_t min_value = defaults::kSensorMinValue;
} SensorSettings;

typedef struct {
  uint16_t number_of_bins = defaults::kNumberOfBins - 1;
  uint32_t duration_us = defaults::kSignalDurationUs;
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
      signal_generator_settings.duration_us = input.substring(1).toInt();
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

/**
 * @brief selects a set of parameters for the signal generator from a set of
 * lookup tables. The lookup tables are defined in the lut namespace. To follow
 * the servo logic, we define each lookup table as an array values in the range
 * [0, 180].
 *
 */
static void UpdateSettingsFromLUTs(uint8_t index) {
  if (index > lut::kMaxIndex) {
    index = lut::kMaxIndex;
  }
  signal_generator_settings.number_of_bins = lut::kNumberOfBins[index];
  signal_generator_settings.frequency_hz = lut::kFrequencies[index];
}

}  // namespace settings
}  // namespace sensint

#endif  // SENSINT_SETTINGS_H
