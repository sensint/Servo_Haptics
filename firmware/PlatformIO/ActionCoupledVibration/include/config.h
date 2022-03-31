#ifndef SENSINT_CONFIG_H
#define SENSINT_CONFIG_H

/**
 * @brief This file provides the configuration for the hardware and software
 * components, e.g. pins or ports and parameters for communication protocols.
 * These are the parameters that are not changed during the operation of the
 * system.
 *
 * If you want to change the parameters, you have to change the corresponding
 * values in the file settings.h.
 */

namespace sensint {
namespace config {

//=========== Pins ===========
// used for pressure sensor
// TODO: reset pin to A0 for release
static constexpr uint8_t kAnalogSensingPin = A2;
// used to set the Haptic Servo signal parameters based on a servo position
static constexpr uint8_t kServoInputPin = 23;

/**
 * @brief initialize the pins - this function should be called at least in
 * setup()
 *
 */
static void InitializePins() {
  pinMode(kAnalogSensingPin, INPUT);
  pinMode(kServoInputPin, INPUT_PULLDOWN);
}

// serial communication
static constexpr int kBaudRate = 115200;

}  // namespace config
}  // namespace sensint

#endif  // SENSINT_CONFIG_H
