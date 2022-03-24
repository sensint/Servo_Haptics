/*
 * This sketch is used to measure the latency of a ServoHaptic
 * controller. Here, (electrical) latency is defined by the duration
 * from a sensor input to the signal output at the DAC.
 * 
 *    BENCHMARK-controller             ServoHaptics-controller
 *    ====================             =======================
 *    
 *    common ground :: GND o---------o GND :: common ground
 *    trigger out   ::   2 ---------->  A0 :: sensor input
 *    trigger in    ::   3 <----------   8 :: benchmark out
 *    measure       ::   4 <---------- DAC :: signal output
 */


/*
 * If USE_MICROS is defined the firmware uses the micros() function
 * to measure the time. If it's undefined (i.e. commented out) the
 * firmware uses clock cycles. NOTE, to convert cycles into time you
 * need to know the clock speed of your processor, e.g.
 *  - Teensy3.5: 120MHz (120 cycles per microsecond)
 *  - Teensy4.1: 600MHz (600 cycles per microsecond)
 */
#define USE_MICROS

namespace {
  /*
   * This pin is connected to the ServoHaptic controller's sensor input.
   * Setting it HIGH will trigger the ServoHaptic controller to start a pulse.
   */
  constexpr uint8_t kTriggerOutPin = 2;
  bool is_high = false;
  
  /*
   * This pin is connected to the ServoHaptic controller's benchmark pin.
   * It will be set HIGH for the duration of the pulse.
   */
  constexpr uint8_t kTriggerInPin = 3;

  /*
   * This pin is connected to the ServoHaptic controller's DAC output.
   * To reduce latency due to the signal you might use a square waveform.
   */
  constexpr uint8_t kMeasurePin = 4;

  /*
   * The benchmark will run in a given amount of iterations.
   * Afterwards it stops (enters inifinite loop).
   */
  constexpr uint32_t kIterations = 100;
  uint32_t iteration = 0;
  
  // each iteration pauses for a given duration (blocking)
  constexpr uint32_t kTriggerDelayMs = 100;
}

void setup() {
  while (!Serial && millis() < 5000) ;
  Serial.begin(115200);
    
  pinMode(kTriggerOutPin, OUTPUT);
  pinMode(kTriggerInPin, INPUT_PULLDOWN);
  pinMode(kMeasurePin, INPUT_PULLDOWN);

  digitalWriteFast(kTriggerOutPin, LOW);

  Serial.println("Benchmark");
#ifdef USE_MICROS
  Serial.println(" >>> time measurement in microseconds");
#else
  Serial.println(" >>> time measurement in clock cycles");
#endif
}

void loop() {
  if (iteration < kIterations) {
    /*
     * Toggle the pin from LOW to HIGH and vice versa to
     * trigger the ServoHaptic controller's sensor input.
     * Each trigger will 
     */
    digitalWriteFast(kTriggerOutPin, (is_high) ? LOW : HIGH);

    /*
     * Start the time measurement.
     */
#ifdef USE_MICROS
    uint32_t time = micros();
#else
    uint32_t cycles = ARM_DWT_CYCCNT;
#endif

    /*
     * Wait for the signal from the ServoHaptic controller's DAC output
     * to go HIGH. Since the output is an analog signal it maybe has some
     * latency to reach the HIGH-threshold voltage. You can try to reduce
     * this latency by using a square wave.
     */
    while (!digitalReadFast(kMeasurePin)) ;

    /*
     * Stop the time measurement as soon as the signal was recognized.
     */
#ifdef USE_MICROS
    uint32_t delta_time = micros() - time;
#else
    uint32_t delta_cycles = ARM_DWT_CYCCNT - cycles - 2;
#endif

    /*
     * Write result of this iteration to the serial port.
     */
#ifdef USE_MICROS
    Serial.print(delta_time);
#else
    Serial.print(delta_cycles);
#endif
    Serial.print(',');

    /* 
     * Wait for ServoHaptic controller to finish the pulse (i.e. go to LOW). 
     */
    while (digitalReadFast(kTriggerInPin)) ;

    /*
     * Wait a given time before we start the next iteration.
     */
    delay(kTriggerDelayMs);
    iteration++;
  } else {
    /*
     * After running all iterations the firmware will stop, i.e. enter an infinite loop.
     */
    Serial.printf("\n >>> Finished benchmark with %d iterations.\n", (int)kIterations);
    while (true) ;
  }
}
