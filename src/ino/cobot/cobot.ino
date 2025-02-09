#include <Servo.h>

/*
  HC-SR04 ultrasonic module 
 * Wiring: Ultrasonic Sensor -> Arduino:
 * - VCC  -> 5VDC
 * - TRIG -> Pin 10
 * - ECHO -> Pin 11
 * - GND  -> GND
 * 
 * SG90 Micro Servo Motors 4pcs
 * Wiring: Servo -> Arduino:
 * - VCC  -> 5VDC
 * - GND  -> GND
 * - PWM -> Pins (3, 5, 6, 9)
 * Orientation
* Left servos 180: Back; 0 Front
* Right servos 180 Front; 0 Back.
*/

Servo backLeftServo;
Servo backRightServo;
Servo frontLeftServo;
Servo frontRightServo;
int trigPin = 10;
int echoPin = 11;
int rest = 90;

//Speed of sound to CM conversion
const float SOS_CM = 0.017;

void setup() {
  //Attach the servos
  backLeftServo.attach(6);
  backRightServo.attach(5);
  frontLeftServo.attach(9);
  frontRightServo.attach(3);

  //Set the pin modes for the HC-SR04 ultrasonic module 
  pinMode(trigPin, OUTPUT);  
  pinMode(echoPin, INPUT);

  // begin serial port to debug.
  Serial.begin(9600);


  // Start by setting all servos to rest position
  stand();
  delay(300); //wait for the servos to reach their place
}

//Function to calculate the distance to an obstacle
float ping () {
  //10 micro-second pulse
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  return SOS_CM * pulseIn(echoPin, HIGH);
}

//wave with the front right servo
void wave() {
  frontRightServo.write(180);
  delay(500);
  for( int i = 0; i <= 2; i++) {
    frontRightServo.write(120);
    delay(300);
    frontRightServo.write(180);
    delay(300);
  }
  frontRightServo.write(rest);
  delay(500);
}

void turnRight() {
  frontLeftServo.write(0);
  backLeftServo.write(20);
  delay(200);
  backRightServo.write(0);
  frontRightServo.write(40);
  delay(200);
  frontLeftServo.write(rest);
  backLeftServo.write(rest);
  backRightServo.write(rest);
  frontRightServo.write(rest);
  delay(200);
}

//hind legs for forward just a bit, 
void sit() {
  frontLeftServo.write(rest);
  backLeftServo.write(40);
  backRightServo.write(140);
  frontRightServo.write(rest);
}

void stand() {
  backLeftServo.write(rest);
  backRightServo.write(rest);
  frontLeftServo.write(rest);
  frontRightServo.write(rest);
}

void layDown() {
  backLeftServo.write(0);
  backRightServo.write(180);
  frontLeftServo.write(0);
  frontRightServo.write(180);
}
 
void walkForward() {
  // Move front left forward
  frontLeftServo.write(0); // Forward
  delay(200);
  frontLeftServo.write(rest); // Return to rest
  delay(200);

  // Move back right forward
  backRightServo.write(180); // Forward
  delay(200);
  backRightServo.write(rest); // Return to rest
  delay(200);

  // Move front right forward
  frontRightServo.write(180); // Forward
  delay(200);
  frontRightServo.write(rest); // Return to rest
  delay(200);

  // Move back left forward
  backLeftServo.write(0); // Forward
  delay(200);
  backLeftServo.write(rest); // Return to rest
  delay(200);
}


void loop() {

  if (Serial.available() > 0)
  {
    String command = Serial.readStringUntil('\n'); // Read until newline
    command.trim();                                // Remove any leading/trailing whitespace

    if (command == "sit")
    {
      sit();
      Serial.println("Arduino: Sitting");
    }
    else if (command == "stand")
    {
      stand();
      Serial.println("stand");
    }
    else
    {
      Serial.println("Arduino: Unknown command: " + command);
    }
  }
  // //float distanceToObstacle = ping();
  // //Serial.println(distanceToObstacle);
  // if (distanceToObstacle > 10) {
  //   //walkForward();
  // } else {
  //   //wave();
  //   //turnRight();
  // }
  //TODO: create a slower loop for the ultrasonic sensor.

  //sit();
  //delay(100);
}
