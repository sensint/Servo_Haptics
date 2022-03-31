#include <Servo.h>


namespace {
Servo haptic_servo;
static constexpr uint8_t servo_pin = 9;

static constexpr uint8_t max_angle = 180;
uint8_t current_angle = 0;
}


void setup() {
  while (!Serial && millis() < 5000) ;
  Serial.begin(115200);
  Serial.println("Haptic Servo Demo");
  
  haptic_servo.attach(servo_pin);
}


void loop() {
  Serial.printf("set Haptic Servo to %i degree\n", current_angle);
  
  haptic_servo.write(current_angle);
  current_angle = (current_angle == max_angle) ? 0 : current_angle+1;
  
  delay(1000);
}
