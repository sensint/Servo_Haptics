#ifndef SENSINT_BENCHMARK_H
#define SENSINT_BENCHMARK_H

#if SENSINT_BENCHMARK_MODE == 1
#define SENSINT_BENCHMARK_MICROS
#elif SENSINT_BENCHMARK_MODE == 2
#define SENSINT_BENCHMARK_CYCLES
#elif SENSINT_BENCHMARK_MODE == 3
#define SENSINT_BENCHMARK_OSCI
#elif SENSINT_BENCHMARK_MODE == 4
#define SENSINT_BENCHMARK_EXTERNAL
#else
#undef SENSINT_BENCHMARK
#endif  // SENSINT_BENCHMARK_MODE

#include <Arduino.h>

namespace sensint {
namespace benchmark {

static constexpr uint32_t kIterations = 1000;
uint32_t iterations = 0;

#ifdef SENSINT_BENCHMARK_MICROS
elapsedMicros time_us;
#endif  // SENSINT_BENCHMARK_MICROS
#ifdef SENSINT_BENCHMARK_CYCLES
uint32_t cycles;
#endif  // SENSINT_BENCHMARK_CYCLES
#if defined(SENSINT_BENCHMARK_OSCI) || defined(SENSINT_BENCHMARK_EXTERNAL)
static constexpr uint8_t pin = 8;
#endif  // SENSINT_BENCHMARK_OSCI || SENSINT_BENCHMARK_EXTERNAL

inline void Initialize() __attribute__((always_inline));
inline void Start() __attribute__((always_inline));
inline void Finish() __attribute__((always_inline));

void Initialize() {
#ifdef SENSINT_BENCHMARK_MICROS
  time_us = 0;
#endif  // SENSINT_BENCHMARK_MICROS
#ifdef SENSINT_BENCHMARK_CYCLES
  cycles = ARM_DWT_CYCCNT;
#endif  // SENSINT_BENCHMARK_CYCLES
#ifdef SENSINT_BENCHMARK_OSCI
  pinMode(pin, OUTPUT);
#endif  // SENSINT_BENCHMARK_OSCI
#ifdef SENSINT_BENCHMARK_EXTERNAL
  pinMode(pin, OUTPUT);
  digitalWriteFast(pin, LOW);
#endif  // SENSINT_BENCHMARK_EXTERNAL
}

void Start() {
#ifdef SENSINT_BENCHMARK_MICROS
  time_us = 0;
#endif  // SENSINT_BENCHMARK_MICROS
#ifdef SENSINT_BENCHMARK_CYCLES
  cycles = ARM_DWT_CYCCNT;
#endif  // SENSINT_BENCHMARK_CYCLES
#if defined(SENSINT_BENCHMARK_OSCI) || defined(SENSINT_BENCHMARK_EXTERNAL)
  digitalWriteFast(pin, HIGH);
#endif  // SENSINT_BENCHMARK_OSCI || SENSINT_BENCHMARK_EXTERNAL
}

void Finish() {
#ifdef SENSINT_BENCHMARK_MICROS
  // Serial.printf("time us: %u\n", (int)time_us);
  Serial.print(time_us);
  Serial.print(",");
#endif  // SENSINT_BENCHMARK_MICROS
#ifdef SENSINT_BENCHMARK_CYCLES
  // https://forum.pjrc.com/threads/61561-Teensy-4-Global-vs-local-variables-speed-of-execution?highlight=execution
  cycles = ARM_DWT_CYCCNT - cycles - 2;
  // Serial.printf("cycles: %u\n", cycles);
  Serial.print(cycles);
  Serial.print(",");
#endif  // SENSINT_BENCHMARK_CYCLES
#if defined(SENSINT_BENCHMARK_OSCI) || defined(SENSINT_BENCHMARK_EXTERNAL)
  digitalWriteFast(pin, LOW);
#else
  iterations++;
  if (iterations == kIterations) {
    Serial.println("done");
    Serial.flush();
    while (true) {
      // wait forever
    }
  }
#endif  // SENSINT_BENCHMARK_OSCI || SENSINT_BENCHMARK_EXTERNAL
}

// this data is used to generate the benchmark data with a constant value
// #define SENSINT_ARB_WAVE
#ifdef SENSINT_ARB_WAVE
constexpr int16_t arb_wave_data[256] = {
    32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767,
    32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767,
    32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767,
    32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767,
    32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767,
    32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767,
    32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767,
    32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767,
    32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767,
    32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767,
    32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767,
    32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767,
    32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767,
    32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767,
    32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767,
    32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767,
    32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767,
    32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767,
    32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767,
    32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767,
    32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767,
    32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767,
    32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767, 32767,
    32767, 32767, 32767};
#endif  // SENSINT_ARB_WAVE

}  // namespace benchmark
}  // namespace sensint

#endif  // SENSINT_BENCHMARK_H
