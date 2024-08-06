/*
  use a HC-SR04 ultrasonic module 
 * Wiring: Ultrasonic Sensor -> Arduino:
 * - VCC  -> 5VDC
 * - TRIG -> Pin 9
 * - ECHO -> Pin 8
 * - GND  -> GND
 */

//Speed of sound to CM conversion
const float SOS_CM = 0.017;

int trigPin = 9;
int echoPin = 8;

float ping () {
  //10 micro-second pulse
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  return SOS_CM * pulseIn(echoPin, HIGH);
}

void setup() {
  // begin serial port
  Serial.begin (9600);

  pinMode(trigPin, OUTPUT);  
  pinMode(echoPin, INPUT);
}

void loop() {
  Serial.println(ping());
  delay(100);
}
