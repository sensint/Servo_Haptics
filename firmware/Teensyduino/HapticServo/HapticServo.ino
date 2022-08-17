#include <Audio.h>
#include <Wire.h>
#include <cmath>


#define VERSION "v1.0.0"

// uncomment to enable printing debug information to the serial port
#define DEBUG


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
static constexpr uint16_t kNumberOfBins = 20;
static constexpr uint32_t kSignalDurationUs = 10000; // in microseconds considered changing to phase
static constexpr short kSignalWaveform = static_cast<short>(Waveform::kSine);
static constexpr float kSignalFreqencyHz = 150.f;
static constexpr float kSignalAmp = 1.f;

//=========== sensor ===========
static constexpr uint8_t kAnalogSensingPin = A1;
static constexpr float kFilterWeight = 0.05;
static constexpr uint8_t kSensorResolution = 10;
static constexpr uint32_t kSensorMaxValue = (1U << kSensorResolution) - 1;
static constexpr uint32_t kSensorMinValue = 0;
static constexpr uint32_t kSensorJitterThreshold = 7; // increase value if vibration starts resonating too much

//=========== serial ===========
static constexpr int kBaudRate = 115200;

//=========== I2C ===========
static constexpr uint8_t kI2CAddress = 20;

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
  float amp = defaults::kSignalAmp;
} SignalGeneratorSettings;

union SerializableSignalGeneratorSettings {
  SignalGeneratorSettings data;
  uint8_t serialized[sizeof(SignalGeneratorSettings)];
};

//=========== settings instances ===========
// These instances are used to access the settings in the main code.
SensorSettings sensor_settings;
SignalGeneratorSettings signal_generator_settings;
union SerializableSignalGeneratorSettings signal_generator_settings_serialized = { .data = signal_generator_settings };

//=========== audio variables ===========
AudioSynthWaveform signal;
AudioOutputPT8211 dac;
AudioConnection patchCord1(signal, 0, dac, 0);
AudioConnection patchCord2(signal, 0, dac, 1);

//=========== sensor variables ===========
float filtered_sensor_value = 0.f;

//=========== control flow variables ===========
elapsedMicros pulse_time_us = 0;
bool is_vibrating = false;
uint16_t last_bin_id = 0;

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
inline void SetupSerial() __attribute__((always_inline));
inline void SetupAudio() __attribute__((always_inline));
inline void SetupSensor() __attribute__((always_inline));
inline void SetupI2C() __attribute__((always_inline));
inline void SetupServo() __attribute__((always_inline));
inline void StartPulse() __attribute__((always_inline));
inline void StopPulse() __attribute__((always_inline));
inline void HandleServoPulse() __attribute__((always_inline));
inline void ApplyHardwareFix() __attribute__((always_inline));
void ServoPinChangingEdge();
void UpdateSettingsFromLUTs(uint8_t index);
void HandleI2COnReceive(int number_of_bytes);

void SetupSerial() {
  while (!Serial && millis() < 5000)
    ;
  Serial.begin(defaults::kBaudRate);
}

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

void SetupSensor() {
  pinMode(defaults::kAnalogSensingPin, INPUT);
  
  analogReadRes(sensor_settings.resolution);
#ifdef DEBUG
  Serial.printf(">>> Set up analog sensor \n\t pin: %d \n\t res: %d bit \n\t range: [%d, %d]\n",
                (int)defaults::kAnalogSensingPin,
                (int)sensor_settings.resolution,
                (int)sensor_settings.min_value,
                (int)sensor_settings.max_value);
#endif
}

void SetupI2C() {
  Wire.begin(defaults::kI2CAddress);
  Wire.onReceive(HandleI2COnReceive);
#ifdef DEBUG
  Serial.printf(">>> Set up I2C \n\t addr: %d \n", (int)defaults::kI2CAddress);
#endif
}

void SetupServo() {
  pinMode(defaults::kServoInputPin, INPUT_PULLUP);
  attachInterrupt(defaults::kServoInputPin, ServoPinChangingEdge, CHANGE);
#ifdef DEBUG
  Serial.printf(">>> Set up servo \n\t pin: %d \n", (int)defaults::kServoInputPin);
#endif
}

/**
 * @brief start a pulse by setting the amplitude of the signal to a predefined
 * value
 *
 */
void StartPulse() {
  signal.begin(signal_generator_settings.waveform);
  signal.frequency(signal_generator_settings.frequency_hz);
  signal.phase(0.0);
  signal.amplitude(signal_generator_settings.amp);
  pulse_time_us = 0;
  is_vibrating = true;
#ifdef DEBUG
  Serial.printf(">>> Start pulse \n\t wave: %d \n\t amp: %.2f \n\t freq: %.2f Hz \n\t dur: %d µs\n",
                (int)signal_generator_settings.waveform,
                signal_generator_settings.amp,
                signal_generator_settings.frequency_hz,
                (int)signal_generator_settings.duration_us);
#endif
}

/**
 * @brief stop the pulse by setting the amplitude of the signal to zero
 *
 */
void StopPulse() {
  signal.amplitude(0.f);
  is_vibrating = false;
#ifdef DEBUG
  Serial.println(F(">>> Stop pulse"));
#endif
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
#ifdef DEBUG
      Serial.printf(">>> New servo angle \n\t angle: %d \n", (int)servo_angle);
#endif
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
#ifdef DEBUG
  Serial.printf(">>> Update settings from LUTs \n\t number of bins: %d \n\t frequency: %.2f \n", 
                (int) signal_generator_settings.number_of_bins,
                signal_generator_settings.frequency_hz);
#endif
}


void HandleI2COnReceive(int number_of_bytes) {
  int idx = 0;
  while (Wire.available()) {
    signal_generator_settings_serialized.serialized[idx] = Wire.read();
    idx++;
  }
  if (idx == (sizeof(SignalGeneratorSettings)-1)) {
    signal_generator_settings = signal_generator_settings_serialized.data;
  }
}

//! This should be removed for the next PCB version!
void ApplyHardwareFix() {
  pinMode(A2,OUTPUT);
  digitalWrite(A2, HIGH);
  pinMode(2,OUTPUT);
  digitalWrite(2, LOW);
}

}  // namespace

void setup() {
  SetupSerial();
  
#ifdef DEBUG
  Serial.printf("HAPTIC SERVO (%s)\n\n", VERSION);
  Serial.println(F("======================= SETUP ======================="));
#endif

  SetupI2C();
  SetupAudio();
  SetupSensor();
  SetupServo();

  //! This should be removed for the next PCB version!
  ApplyHardwareFix();

  // initialize the system assuming the servo being at 0°
  //signal_generator_settings.number_of_bins = lut::kNumberOfBins[0];
  //signal_generator_settings.frequency_hz = lut::kFrequencies[0];

#ifdef DEBUG
  Serial.printf(">>> Signal generator settings \n\t bins: %d \n\t wave: %d \n\t amp: %.2f \n\t freq: %.2f Hz \n\t dur: %d µs\n",
                (int)signal_generator_settings.number_of_bins,
                (int)signal_generator_settings.waveform,
                signal_generator_settings.amp,
                signal_generator_settings.frequency_hz,
                (int)signal_generator_settings.duration_us);
  Serial.println(F("=====================================================\n\n"));
#endif

  delay(500);
}



void loop() {
  //! This is for testing only!
  /*
  static elapsedMillis servo_test_timer = 0;
  if (servo_test_timer > 2000) {
    static uint8_t test_angle = 0;
    UpdateSettingsFromLUTs((test_angle++)%180);
    servo_test_timer = 0;
  }
  */
  
  if (is_new_servo_pulse && servo_timer_ms > defaults::kServoDelayMs) {
    HandleServoPulse();
    servo_timer_ms = 0;
  }
  
  auto sensor_value = analogRead(defaults::kAnalogSensingPin);
  filtered_sensor_value =
      (1.f - sensor_settings.filter_weight) * filtered_sensor_value +
      sensor_settings.filter_weight * sensor_value;
  static uint16_t last_triggered_sensor_val = filtered_sensor_value;

  // calculate the bin id depending on the filtered sensor value
  // (currently linear mapping)
  uint16_t mapped_bin_id = map(filtered_sensor_value, sensor_settings.min_value,
                               sensor_settings.max_value, 0,
                               signal_generator_settings.number_of_bins);
  
  if (mapped_bin_id != last_bin_id) {
    auto dist = std::abs(filtered_sensor_value - last_triggered_sensor_val);
    if (dist < defaults::kSensorJitterThreshold) {
       return;
    }
    
    if (is_vibrating) {
#ifdef DEBUG
      Serial.println(F(">>> Stop pulse before it finished"));
#endif
      StopPulse();
      delay(1); // debatable ;) maybe use delayMicroseconds(100) instead
    }
    
#ifdef DEBUG
    Serial.printf(">>> Change bin \n\t bin id: %d\n", (int)mapped_bin_id);
#endif
    StartPulse();
    last_bin_id = mapped_bin_id;
    last_triggered_sensor_val = filtered_sensor_value;
  }

  if (is_vibrating && pulse_time_us >= signal_generator_settings.duration_us) {
    StopPulse(); //stop pulse if duration is exceeded
  }  
}
