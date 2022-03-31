#ifndef SENSINT_DEBUG_H
#define SENSINT_DEBUG_H

#include <Arduino.h>

namespace sensint {
namespace debug {

/**
 * @brief levels of debug output
 * none: no debug output
 * basic: can be used for control flow
 * verbose: can be used for detailed information
 */
enum class DebugLevel { none, basic, verbose };

#if SENSINT_DEBUG == 0
#undef SENSINT_DEBUG
const auto kDebugLevel = DebugLevel::none;
#else
const auto kDebugLevel =
    (SENSINT_DEBUG == 1) ? DebugLevel::basic : DebugLevel::verbose;
#endif  // SENSINT_DEBUG

#ifdef SENSINT_DEBUG

/**
 * @brief log a message to the serial port
 *
 * @param message the message to log
 * @param level the level of the message
 */
static void Log(const String& message,
                const DebugLevel level = DebugLevel::basic) {
  if (static_cast<uint8_t>(level) <= static_cast<uint8_t>(kDebugLevel)) {
    Serial.println(message);
  }
}

/**
 * @brief log a message to the serial port
 *
 * @param func the function where the log was called
 * @param message the message to log
 * @param level the level of the message
 */
static void Log(const String& func, const String& message,
                const DebugLevel level = DebugLevel::basic) {
  if (static_cast<uint8_t>(level) <= static_cast<uint8_t>(kDebugLevel)) {
    Serial.println(func + " >>> " + message);
  }
}

/**
 * @brief log a message to the serial port
 *
 * @param func the function where the log was called
 * @param line the line where the log was called
 * @param message the message to log
 * @param level the level of the message
 */
static void Log(const String& func, const int line, const String& message,
                const DebugLevel level = DebugLevel::basic) {
  if (static_cast<uint8_t>(level) <= static_cast<uint8_t>(kDebugLevel)) {
    Serial.println(func + ":l" + line + " >>> " + message);
  }
}

/**
 * @brief log a message to the serial port
 *
 * @param file the file where the log was called
 * @param func the function where the log was called
 * @param line the line where the log was called
 * @param message the message to log
 * @param level the level of the message
 */
static void Log(const String& file, const String& func, const int line,
                const String& message,
                const DebugLevel level = DebugLevel::basic) {
  if (static_cast<uint8_t>(level) <= static_cast<uint8_t>(kDebugLevel)) {
    Serial.println(file + " @ " + func + ":l" + line + " >>> " + message);
  }
}

#endif  // SENSINT_DEBUG

}  // namespace debug
}  // namespace sensint

#endif  // SENSINT_DEBUG_H
