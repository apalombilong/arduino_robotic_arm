# Arduino Robotic Arm

A six-servo robotic arm I built for my ECE 5 project at UC Santa Barbara. It uses an Arduino Uno and a PCA9685 servo driver. The code lets me move each servo through the Serial Monitor, move them smoothly, and return the arm to its home position.

## How to use

1. Install the **Adafruit PWM Servo Driver** library in Arduino IDE.
2. Open `robotic_arm_controller.ino` and upload it to the Arduino.
3. Open the Serial Monitor at **9600 baud** and select **Newline**.

## Commands

- `0 30` — move channel 0 to 30 degrees
- `p` — print the stored servo angles
- `h` — move all servos to their home positions

Channels are numbered **0–5**.
