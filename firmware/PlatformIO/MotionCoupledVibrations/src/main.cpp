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

namespace {

//=========== audio variables ===========
AudioSynthWaveform signal;
AudioOutputPT8211 to_haptuator;
AudioConnection patchCord1(signal, 0, to_haptuator, 0);
AudioConnection patchCord2(signal, 0, to_haptuator, 1);

//=========== sensor variables ===========
float filtered_sensor_value = 0.f;

//=========== control flow variables ===========
uint32_t pulse_start_time = 0;
bool is_vibrating = false;
uint16_t last_bin_id = 0;
uint16_t new_pulse_id = 0;
uint16_t current_pulse_id = 0;

//=========== helper functions ===========
// These functions were extracted to simplify the control flow and will be
// inlined by the compiler.

/**
 * @brief set up the audio system
 *
 */
inline void SetupAudio() {
  AudioMemory(20);
  delay(50);  // time for DAC voltage stable
  signal.begin(sensint::settings::signal_generator_settings.waveform);
  signal.frequency(sensint::settings::signal_generator_settings.frequency_hz);
}

/**
 * @brief start a pulse by setting the amplitude of the signal to a predefined
 * value
 *
 */
inline void StartPulse() {
  pulse_start_time = millis();
  signal.amplitude(sensint::settings::signal_generator_settings.amp_pos);
  is_vibrating = true;
#ifdef SENSINT_DEBUG
  sensint::debug::Log("start pulse");
#endif  // SENSINT_DEBUG
}

/**
 * @brief stop the pulse by setting the amplitude of the signal to zero
 *
 */
inline void StopPulse() {
  signal.amplitude(0.f);
  is_vibrating = false;
#ifdef SENSINT_DEBUG
  sensint::debug::Log("stop pulse");
#endif  // SENSINT_DEBUG
}

}  // namespace

/**
 * @brief this function is called only once at the beginning of the program
 *
 */
void setup() {
  using namespace sensint;

#ifdef SENSINT_DEVELOPMENT
  Serial.begin(config::kBaudRate);
#ifdef SENSINT_DEBUG
  debug::Log(FW_NAME);
  debug::Log(GIT_TAG);
  debug::Log(GIT_REV);
#endif  // SENSINT_DEBUG
#endif  // SENSINT_DEVELOPMENT

  config::InitializePins();
  analogReadRes(settings::sensor_settings.resolution);
  SetupAudio();
}

/**
 * @brief this function is called repeatedly during the program
 *
 */
void loop() {
  using namespace sensint;

#ifdef SENSINT_DEVELOPMENT
  settings::UpdateSettingsFromSerialInput();
#endif  // SENSINT_DEVELOPMENT

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
    if (millis() >=
        (pulse_start_time + settings::signal_generator_settings.duration_ms)) {
      StopPulse();
    }
  }
}
