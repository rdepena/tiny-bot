# Tiny-bot:
A super simple robot project designed to get my kids excited about electronics and robotics.
<img src="https://raw.githubusercontent.com/rdepena/tiny-bot/refs/heads/main/assets/ezgif-3997eabecf6e73.gif">

## Chasis:
The robot uses a 3D-printed chassis based on the [COBOT chasis](https://makerworld.com/en/models/523448#profileId-440067) by Luca Dilo.
* Cute and fun – the kids love it!
* Easy to print – perfect for quick assembly.

## Parts used:
This project is built with a few basic and affordable components:
* 1x HC-SR04 ultrasonic module (for distance sensing)
* 4x SG90 micro servo motors (for movement)
* 1x [Microduino core](https://www.wiki.microduinoinc.com/Microduino-Module_Core) (for processing)

## Software 
Tiny-bot runs on two main software components:

### Arduino (Robot Logic)
The robot is programmed to walk and avoid obstacles autonomously:

* If an obstacle is too close, it turns right automatically.
* Ultrasonic sensor data is smoothed using a moving average filter.

The bot also supports serial commands, allowing it to:
* Sit
* Wave
* Stretch

### Companion App
Since the robot only has one ultrasonic sensor, I built a companion application using SDL2 to:

* Visualize sensor data on a vintage oscilloscope-style grid.
* Send remote commands to control the robot.

Screenshot:
<img src="https://raw.githubusercontent.com/rdepena/tiny-bot/refs/heads/main/assets/comp-app.png">

## Dependencies: 
To run this project, you'll need the following:
For the SDL UI:

* pkg-config → Install via brew install pkg-config
* sdl2 → Install via brew install sdl2

For the Arduino Code:

Arduino IDE (to upload the code to the Microduino)
