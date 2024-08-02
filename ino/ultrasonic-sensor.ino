/*
 * Wiring: Ultrasonic Sensor -> Arduino:
 * - VCC  -> 5VDC
 * - TRIG -> Pin 9
 * - ECHO -> Pin 8
 * - GND  -> GND
 */

int trigPin = 9;
int echoPin = 8;

void setup() {
  // begin serial port
  Serial.begin (9600);

  pinMode(trigPin, OUTPUT);  
  pinMode(echoPin, INPUT);
}

void loop() {
}
