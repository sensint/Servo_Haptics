/* ANALOG TO PULSE by senSInt (2024)
 *
 * This example demonstrates a simple implementation of a 
 * grain-based compliance illusion. This code uses the Teensy audio
 * library to generate vibrotactile stimuli. To read more about the
 * technical details and what this type of vibrotactile rendering can
 * be used for, please refer to our CHI'23 paper:
 * Haptic Servos: Self-Contained Vibrotactile Rendering System
 * for Creating or Augmenting Material Experiences.
 * https://dl.acm.org/doi/abs/10.1145/3544548.3580716
 */


#include <Audio.h>

// install Rob Tillaart's MultiMap library:
// https://github.com/RobTillaart/MultiMap
#include "MultiMap.h"




/************* AUGMENTATION PARAMETERS - START ********************/
namespace augmentation {
// Set the number of pulses across the sensor range.
// The sensor's range (analog reading) is split into equidistant bins.
static const uint8_t kBins = 30;

// Set the amplitude of the output.
// Ideally, keep the amplitude at the maximum (1.0) and
// tune it using the trim pot on the Haptic Servo shield.
static const float kAmplitude = 1.0;
// Should pulses be rendered differently between press and release?
static const bool kAsymAmp = true;
// Should pulses be rendered with in-/decreasing amplitude?
static const bool kFadeAmp = true;
// The following parameters are only used, if kAsymAmp or kFadeAmp is true.
static const float kAmpMin = 0.3;
static const float kAmpMax = 1.0;

// Set the vibration frequency.
// LRAs have a limited frequency spectrum. Hence, check its datasheet
// to get the resonance frequency for maximum output.
static const uint16_t kFrequency = 100;

// Set the duration of a individual pulses.
// Depending on the mechanical properties of the attached actuator,
// very short pulses might not be possible.
static const uint8_t kPulseDuration = 3;
// Should pulses played with random durations?
static const bool kRandDuration = false;
// The following parameters are only used, if kRandDuration is true.
static const uint8_t kPulseDurationMin = 3;
static const uint8_t kPulseDurationMax = 15;
}
/************* AUGMENTATION PARAMETERS - END ********************/







namespace {
static const float kFilterWeight = 0.007;
static const float kSensorRef[15] = {0.00, 3.33, 6.66, 10.00, 13.33, 16.66, 20.00, 23.33, 26.66, 30.00, 33.33, 50.00, 66.66, 83.33, 100.00};
static const float kSensorRes[15] = {0.00, 51.81, 68.42, 77.22, 81.13, 82.11, 84.07, 86.02, 87.98, 88.47, 88.95, 89.93, 90.91, 91.89, 92.38};

static float sensor_val_filtered = 0;
static int last_bin = 0;
static float last_triggered_pos = 0;
static const float kBinDebounceWidth = 100.0 / augmentation::kBins / 3.0;
static const uint8_t kSensorPin = A1;

AudioSynthWaveform signal;
AudioOutputPT8211 dac;
AudioConnection patchCord1(signal, 0, dac, 0);
AudioConnection patchCord2(signal, 0, dac, 1);
}


void setup() {
  using namespace augmentation;
  AudioMemory(20);
  delay(50);
  signal.begin(WAVEFORM_SINE);
  signal.frequency(kFrequency);
  Serial.begin(115200);
  pinMode(kSensorPin, INPUT);
  pinMode(2, OUTPUT);
  digitalWrite(2, HIGH);
  delay(20);

  Serial.printf("\n\n--- ANALOG TO PULSE ---\n\n");
}


void loop() {
  using namespace augmentation;
  sensor_val_filtered = ((1.0 - kFilterWeight) * sensor_val_filtered)
                          + (kFilterWeight * analogRead(kSensorPin));
  float sensor_val_percent = multiMap<const float>(
                              sensor_val_filtered/10.23,
                              kSensorRes,
                              kSensorRef,
                              15);
  int bin = map(sensor_val_percent, 0.f, 100.f, 0, kBins);
  
  if (bin == last_bin || abs(last_triggered_pos - sensor_val_percent) < kBinDebounceWidth) {
    return;
  }

  float amp = kAmplitude;

  if (kAsymAmp) {
    if (kFadeAmp) {
      if (sensor_val_percent < last_triggered_pos) {
        amp = map(sensor_val_percent, 0.f, 100.f, 0.f, kAmpMin);
      } else {
        amp = map(sensor_val_percent, 0.f, 100.f, kAmpMin, kAmpMax);
      }
    } else {
      amp = (sensor_val_percent < last_triggered_pos) ? kAmpMin : kAmpMax;
    }
  } else {
    amp = (kFadeAmp) ? map(sensor_val_percent, 0.f, 100.f, kAmpMin, kAmpMax) : kAmplitude;
  }
  
  signal.amplitude(amp);

  if (kRandDuration) {
    delay(random(kPulseDurationMin, kPulseDurationMax));
  } else {
    delay(kPulseDuration);
  }

  signal.amplitude(0.f);
  
  last_bin = bin;
  last_triggered_pos = sensor_val_percent;
}
