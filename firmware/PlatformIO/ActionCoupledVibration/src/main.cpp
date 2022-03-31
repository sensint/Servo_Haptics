//=========== system headers ===========
#include <Arduino.h>

//=========== library headers ===========
#include <Audio.h>

//=========== project headers ===========
#include "build.h"
#include "config.h"
#include "settings.h"
#ifdef SENSINT_DEVELOPMENT
// debug messages are only available in the development build
#include "debug.h"
#endif  // SENSINT_DEVELOPMENT
#ifdef SENSINT_BENCHMARK
#include "benchmark.h"
#endif  // SENSINT_BENCHMARK

namespace {

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

/**
 * @brief set up the audio system
 *
 */
void SetupAudio() {
  AudioMemory(20);
  delay(50);  // time for DAC voltage stable
#ifdef SENSINT_ARB_WAVE
  signal.begin(1.f, 40.f,
               static_cast<short>(sensint::settings::Waveform::kArbitrary));
  signal.arbitraryWaveform(sensint::benchmark::arb_wave_data, 40.f);
  signal.amplitude(0.f);
#else
  signal.begin(sensint::settings::signal_generator_settings.waveform);
  signal.frequency(sensint::settings::signal_generator_settings.frequency_hz);
#endif  // SENSINT_ARB_WAVE
}

/**
 * @brief start a pulse by setting the amplitude of the signal to a predefined
 * value
 *
 */
void StartPulse() {
  pulse_time_us = 0;
#ifdef SENSINT_ARB_WAVE
  // signal.begin(sensint::settings::signal_generator_settings.amp_pos, 40.f,
  //              static_cast<short>(sensint::settings::Waveform::kArbitrary));
  // signal.arbitraryWaveform(arb_wave_data, 40.f);
  signal.amplitude(sensint::settings::signal_generator_settings.amp_pos);
#else
  signal.amplitude(sensint::settings::signal_generator_settings.amp_pos);
  signal.begin(sensint::settings::signal_generator_settings.waveform);
#endif  // SENSINT_ARB_WAVE
  is_vibrating = true;
#ifdef SENSINT_DEBUG
  sensint::debug::Log("start pulse");
#endif  // SENSINT_DEBUG
}

/**
 * @brief stop the pulse by setting the amplitude of the signal to zero
 *
 */
void StopPulse() {
  signal.amplitude(0.f);
  is_vibrating = false;
#ifdef SENSINT_DEBUG
  sensint::debug::Log("stop pulse");
#endif  // SENSINT_DEBUG
}

void ServoPinChangingEdge() {
  if (digitalReadFast(sensint::config::kServoInputPin)) {
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
    servo_angle =
        (uint8_t)map(servo_pulse_length, kMinServoPulseLength,
                     kMaxServoPulseLength, kMinServoAngle, kMaxServoAngle);
    if (servo_angle != last_servo_angle) {
      sensint::settings::UpdateSettingsFromLUTs(servo_angle);
      last_servo_angle = servo_angle;
#ifdef SENSINT_DEBUG
      sensint::debug::Log("servo angle " + String((int)servo_angle),
                          sensint::debug::DebugLevel::verbose);
#endif  // SENSINT_DEBUG
    }
  }
}

}  // namespace

void setup() {
  using namespace sensint;

#if defined(SENSINT_DEVELOPMENT) || defined(SENSINT_BENCHMARK)
  while (!Serial && millis() < 5000)
    ;
  Serial.begin(config::kBaudRate);
  Serial.printf("\n\n================================================\n");
  Serial.printf("Firmware: %s\n", FW_NAME);
  Serial.printf(">>> version: %s\n", GIT_TAG);
  Serial.printf(">>> revision: %s\n", GIT_REV);
#ifdef SENSINT_DEBUG
  Serial.println(">>> debugging enabled");
#endif  // SENSINT_DEBUG
#ifdef SENSINT_BENCHMARK
  Serial.println(">>> benchmarking enabled");
#endif  // SENSINT_BENCHMARK
  Serial.printf("================================================\n");
#endif  // SENSINT_DEVELOPMENT || SENSINT_BENCHMARK

  config::InitializePins();
  analogReadRes(settings::sensor_settings.resolution);
  SetupAudio();

  attachInterrupt(config::kServoInputPin, ServoPinChangingEdge, CHANGE);

#ifdef SENSINT_BENCHMARK
  benchmark::Initialize();
#endif  // SENSINT_BENCHMARK
}

void loop() {
  using namespace sensint;

#ifdef SENSINT_DEVELOPMENT
  settings::UpdateSettingsFromSerialInput();
#endif  // SENSINT_DEVELOPMENT

  if (is_new_servo_pulse &&
      servo_timer_ms > settings::defaults::kServoDelayMs) {
    HandleServoPulse();
    servo_timer_ms = 0;
  }

  // read the sensor value and filter it
  auto sensor_value = analogRead(config::kAnalogSensingPin);
  filtered_sensor_value =
      (1.f - settings::sensor_settings.filter_weight) * filtered_sensor_value +
      settings::sensor_settings.filter_weight * sensor_value;

  // calculate the bin id depending on the filtered sensor value
  // (currently linear mapping)
  uint16_t mapped_bin_id =
      map(filtered_sensor_value, settings::sensor_settings.min_value,
          settings::sensor_settings.max_value, 0,
          settings::signal_generator_settings.number_of_bins);

  if (mapped_bin_id != last_bin_id) {
    //! bin CHANGED
#ifdef SENSINT_DEBUG
    debug::Log("bin " + String((int)mapped_bin_id), debug::DebugLevel::verbose);
#endif  // SENSINT_DEBUG

    // ******* #1 Assign a pulse number *************************
    // check for the pulse ID
    if (mapped_bin_id > last_bin_id) {
      new_pulse_id = mapped_bin_id;
    } else {
      new_pulse_id = mapped_bin_id;
    }

    // ******* #2 Check if Pulse is already vibrating *************************
    // if it's not vibrating, start a new pulse with a new ID
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
#ifdef SENSINT_BENCHMARK
      benchmark::Start();
#endif  // SENSINT_BENCHMARK
    }

    last_bin_id = mapped_bin_id;
  }

  if (is_vibrating) {
    if (pulse_time_us >= settings::signal_generator_settings.duration_us) {
      StopPulse();
#ifdef SENSINT_BENCHMARK
      benchmark::Finish();
#endif  // SENSINT_BENCHMARK
    }
  }
}
