#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <math.h>

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

#define SERVOMIN 150
#define SERVOMAX 600
#define NUM_SERVOS 6

// Channel assignments
#define BASE_CHANNEL      0
#define SHOULDER_CHANNEL  1
#define ELBOW_CHANNEL     2
#define GRIPPER_CHANNEL   3
#define EXTRA1_CHANNEL    4
#define EXTRA2_CHANNEL    5

// Home positions
#define BASE_HOME       90
#define SHOULDER_HOME   90
#define ELBOW_HOME      90
#define GRIPPER_HOME    90
#define EXTRA1_HOME     90
#define EXTRA2_HOME     90

// Bigger delay = slower / smoother / less jolting
#define SERVO_STEP_DELAY 20

int servoAngles[NUM_SERVOS] = {90, 90, 90, 90, 90, 90};

// ─── Pulse conversion ─────────────────────────────────────────

int angleToPulse(int angle) {
  return map(angle, 0, 180, SERVOMIN, SERVOMAX);
}

// ─── Instant raw set ──────────────────────────────────────────

void setServoAngleInstant(int channel, int angle) {
  if (channel < 0 || channel >= NUM_SERVOS) return;

  angle = constrain(angle, 0, 180);
  pwm.setPWM(channel, 0, angleToPulse(angle));
  servoAngles[channel] = angle;
}

// ─── Smooth move to reduce jolting ────────────────────────────

void moveServoSmooth(int channel, int targetAngle) {
  if (channel < 0 || channel >= NUM_SERVOS) return;

  targetAngle = constrain(targetAngle, 0, 180);

  int current = servoAngles[channel];
  if (current == targetAngle) return;

  int step = (targetAngle > current) ? 1 : -1;

  while (current != targetAngle) {
    current += step;
    pwm.setPWM(channel, 0, angleToPulse(current));
    servoAngles[channel] = current;
    delay(SERVO_STEP_DELAY);
  }
}

// ─── Homing routine ───────────────────────────────────────────

void homeServo(int channel, int homeAngle) {
  int current = servoAngles[channel];

  if (current == homeAngle) {
    Serial.print("Channel ");
    Serial.print(channel);
    Serial.println(" already homed.");
    return;
  }

  moveServoSmooth(channel, homeAngle);

  Serial.print("Channel ");
  Serial.print(channel);
  Serial.println(" homed.");
}

void homeAll() {
  Serial.println("Homing all servos...");
  homeServo(EXTRA2_CHANNEL, EXTRA2_HOME);
  homeServo(EXTRA1_CHANNEL, EXTRA1_HOME);
  homeServo(GRIPPER_CHANNEL, GRIPPER_HOME);
  homeServo(ELBOW_CHANNEL, ELBOW_HOME);
  homeServo(SHOULDER_CHANNEL, SHOULDER_HOME);
  homeServo(BASE_CHANNEL, BASE_HOME);
  Serial.println("All servos homed!");
}

// ─── Move servo directly ──────────────────────────────────────

void setDirectAngle(int channel, int angle) {
  if (channel < 0 || channel >= NUM_SERVOS) {
    Serial.println("Invalid channel. Use 0-5.");
    return;
  }

  angle = constrain(angle, 0, 180);
  moveServoSmooth(channel, angle);

  Serial.print("Channel ");
  Serial.print(channel);
  Serial.print(" -> ");
  Serial.print(angle);
  Serial.println(" degrees");
}

// ─── Print current angles ─────────────────────────────────────

void printAllAngles() {
  Serial.println("\n--- Current Servo Angles ---");
  for (int i = 0; i < NUM_SERVOS; i++) {
    Serial.print("Channel ");
    Serial.print(i);
    Serial.print(": ");
    Serial.print(servoAngles[i]);
    Serial.println(" degrees");
  }
  Serial.println("----------------------------");
}

// ─── Setup ────────────────────────────────────────────────────

void setup() {
  Serial.begin(9600);
  pwm.begin();
  pwm.setPWMFreq(60);

  // Start all servos at 90
  for (int i = 0; i < NUM_SERVOS; i++) {
    setServoAngleInstant(i, 90);
    delay(200);
  }

  Serial.println("=== ARM CONTROLLER READY ===");
  Serial.println("Commands:");
  Serial.println("  0 30    -> move channel 0 to 30 degrees");
  Serial.println("  p       -> print angles");
  Serial.println("  h       -> home all servos");
  Serial.println("Channels: 0 1 2 3 4 5");
  Serial.print("Smooth move delay: ");
  Serial.print(SERVO_STEP_DELAY);
  Serial.println(" ms per degree");
  Serial.println();
}

// ─── Loop ─────────────────────────────────────────────────────

void loop() {
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    input.trim();

    if (input.length() == 0) return;

    if (input.equalsIgnoreCase("p")) {
      printAllAngles();
      return;
    }

    if (input.equalsIgnoreCase("h")) {
      homeAll();
      return;
    }

    int spaceIndex = input.indexOf(' ');
    if (spaceIndex == -1) {
      Serial.println("Invalid command. Use: <channel> <angle>");
      return;
    }

    String channelStr = input.substring(0, spaceIndex);
    String angleStr   = input.substring(spaceIndex + 1);

    int channel = channelStr.toInt();
    int angle   = angleStr.toInt();

    if (channel < 0 || channel >= NUM_SERVOS) {
      Serial.println("Invalid channel. Use 0-5.");
      return;
    }

    if (angle < 0 || angle > 180) {
      Serial.println("Invalid angle. Use 0-180.");
      return;
    }

    setDirectAngle(channel, angle);
  }
}
