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

Servo back_left_servo;
Servo back_right_servo;
Servo front_left_servo;
Servo front_right_servo;
int trig_pin = 10;
int echo_pin = 11;
int rest = 90;

const int max_sample = 10;  // Maximum number of samples to smooth
float buffer[max_sample];  // Buffer to store the last readings
int buffer_index = 0;      // Index for circular buffer
int buffer_count = 0;      // Number of values in the buffer

//Speed of sound to CM conversion
const float SOS_CM = 0.017;

void setup() {
  //Attach the servos
  back_left_servo.attach(6);
  back_right_servo.attach(5);
  front_left_servo.attach(9);
  front_right_servo.attach(3);

  //Set the pin modes for the HC-SR04 ultrasonic module 
  pinMode(trig_pin, OUTPUT);  
  pinMode(echo_pin, INPUT);

  // begin serial port to debug.
  Serial.begin(9600);


  // Start by setting all servos to rest position
  stand();
  delay(300); //wait for the servos to reach their place
}

// Function to smooth the readings from the ultrasonic sensor
float smooth(float val) {
    float sum = 0;

    // Store the new value in the buffer (circular)
    buffer[buffer_index] = val;
    buffer_index = (buffer_index + 1) % max_sample;

    // Keep track of how many values are stored
    if (buffer_count < max_sample) {
        buffer_count++;
    }

    // Compute the moving average
    for (int i = 0; i < buffer_count; i++) {
        sum += buffer[i];
    }
    return sum / buffer_count;
}

//Function to calculate the distance to an obstacle
float ping () {
  //10 micro-second pulse
  digitalWrite(trig_pin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig_pin, LOW);
  return SOS_CM * pulseIn(echo_pin, HIGH);
}

//wave with the front right servo
void wave() {
  sit();
  delay(500);
  front_right_servo.write(180);
  delay(500);
  for( int i = 0; i <= 2; i++) {
    front_right_servo.write(120);
    delay(300);
    front_right_servo.write(180);
    delay(300);
  }
  front_right_servo.write(rest);
  delay(500);
}

void turnRight() {
  front_left_servo.write(0);
  back_left_servo.write(20);
  delay(200);
  back_right_servo.write(0);
  front_right_servo.write(40);
  delay(200);
  front_left_servo.write(rest);
  back_left_servo.write(rest);
  back_right_servo.write(rest);
  front_right_servo.write(rest);
  delay(200);
}

//hind legs for forward just a bit, 
void sit() {
  front_left_servo.write(rest);
  back_left_servo.write(40);
  back_right_servo.write(140);
  front_right_servo.write(rest);
}

void stretch() {
  back_left_servo.write(rest);
  back_right_servo.write(rest);
  front_left_servo.write(0);
  front_right_servo.write(180);
}

void stand() {
  back_left_servo.write(rest);
  back_right_servo.write(rest);
  front_left_servo.write(rest);
  front_right_servo.write(rest);
}

void layDown() {
  back_left_servo.write(0);
  back_right_servo.write(180);
  front_left_servo.write(0);
  front_right_servo.write(180);
}
 
//vestigal but useful for future reference.
void walkForward() {
  // Move front left forward
  front_left_servo.write(0); // Forward
  delay(200);
  front_left_servo.write(rest); // Return to rest
  delay(200);

  // Move back right forward
  back_right_servo.write(180); // Forward
  delay(200);
  back_right_servo.write(rest); // Return to rest
  delay(200);

  // Move front right forward
  front_right_servo.write(180); // Forward
  delay(200);
  front_right_servo.write(rest); // Return to rest
  delay(200);

  // Move back left forward
  back_left_servo.write(0); // Forward
  delay(200);
  back_left_servo.write(rest); // Return to rest
  delay(200);
}

void process_remote_command(String command) {
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
  else if (command == "wave")
  {
    wave();
    Serial.println("Arduino: Waving");
  }
  else if (command == "lay-down")
  {
    layDown();
    Serial.println("Arduino: Turning Right");
  }
  else if (command == "stretch")
  {
    stretch();
    Serial.println("Arduino: Walking Forward");
  }
  else
  {
    Serial.println("Arduino: Unknown command: " + command);
  }
}

void loop() {

  if (Serial.available() > 0)
  {

    String command = Serial.readStringUntil('\n');
    command.trim();

    process_remote_command(command);
  }

  float distanceToObstacle = ping();
  float smoothedDistance = smooth(distanceToObstacle);
  Serial.println(smoothedDistance);
  //this is fast enough of a loop
  delay(100);  
}
