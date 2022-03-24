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
constexpr uint8_t kAnalogSensingPin = A2;
// used for debugging and debounce condition
constexpr uint8_t kLedPin = 13;

/**
 * @brief initialize the pins - this function should be called at least in
 * setup()
 *
 */
static void InitializePins() {
  pinMode(kAnalogSensingPin, INPUT);
  pinMode(kLedPin, OUTPUT);
}

// serial communication
constexpr int kBaudRate = 115200;

}  // namespace config
}  // namespace sensint

#endif  // SENSINT_CONFIG_H
