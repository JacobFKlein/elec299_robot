byte left_speed = 126;
byte right_speed = 114; //PWM values from 0 to 255

#include <Servo.h>
#define TILT 9
#define PAN 8
#define GRIP 10
#define PRESSURE A1 //NEED TO MOVE TO ANOTHER ANALOG PIN

#define BUTTON 12

#define LEFT_WHEEL_SPEED 5
#define LEFT_WHEEL_DIRECTION 4
#define RIGHT_WHEEL_SPEED 6
#define RIGHT_WHEEL_DIRECTION 7

#define LEFT_BUMPER 11
#define RIGHT_BUMPER 13

#define DEGREE_TO_MSEC_RATIO 11

char BTin;

void setup() {
  Initialize();
  WaitBumper();
  Serial.begin(115200);
  digitalWrite(LEFT_WHEEL_DIRECTION, HIGH);
  digitalWrite(RIGHT_WHEEL_DIRECTION, HIGH);
}

void loop() {
  //BTin = GetBT();
  digitalWrite(LEFT_WHEEL_DIRECTION, HIGH);
  digitalWrite(RIGHT_WHEEL_DIRECTION, HIGH);
  analogWrite(LEFT_WHEEL_SPEED, 0); /* 1-255 over 200 fairly unstable */
  analogWrite(RIGHT_WHEEL_SPEED, 0);
  delay(5000);
  SPIN(90, 'L');
  switch (BTin) {
    case '1': // go to left one spin, left 90
      break;
    case '2': // go to right one spin, right 90
      break;
    case '3': // go to back one, spin 180
      break;
    default: //otherwise do...
      break;
  }
}

void Initialize() {
  /* Declaring inputs and outputs */
  pinMode(BUTTON, INPUT); 
  pinMode(LEFT_WHEEL_SPEED, INPUT);
  pinMode(LEFT_WHEEL_DIRECTION, INPUT);
  pinMode(RIGHT_WHEEL_SPEED, INPUT);
  pinMode(RIGHT_WHEEL_DIRECTION, INPUT);
  pinMode(LEFT_BUMPER, INPUT);
  pinMode(RIGHT_BUMPER, INPUT);
  
  /* Initializing Servos (so they dont shake) */
  Servo tilt, pan, grab;
  tilt.attach(TILT);
  tilt.write(48);
  pan.attach(PAN);
  pan.write(90);
  grab.attach(GRIP);
  grab.write(80);
  delay(1000);
  tilt.detach();
  pan.detach();
  grab.detach();
}

void WaitBumper() {
  while (digitalRead(LEFT_BUMPER) == LOW &&
    digitalRead(RIGHT_BUMPER) == LOW) {}
  while (digitalRead(LEFT_BUMPER) == HIGH &&
    digitalRead(RIGHT_BUMPER) == HIGH) {}
  delay(100);
}

char GetBT() {
    BTin = ' ';
  if (Serial.available()) {
    BTin = Serial.read();
  }
  return BTin;
}

void SPIN(int degree, char direction) {
  if (direction == 'L') {
    digitalWrite(LEFT_WHEEL_DIRECTION, LOW);
    digitalWrite(RIGHT_WHEEL_DIRECTION, HIGH);
    analogWrite(LEFT_WHEEL_SPEED, left_speed); /* 1-255 over 200 fairly unstable */
    analogWrite(RIGHT_WHEEL_SPEED, right_speed);
    delay(degree*9.78);
    analogWrite(LEFT_WHEEL_SPEED,0); /* 1-255 over 200 fairly unstable */
    analogWrite(RIGHT_WHEEL_SPEED,0);
  }
  else if (direction == 'R') {
    digitalWrite(LEFT_WHEEL_DIRECTION, HIGH);
    digitalWrite(RIGHT_WHEEL_DIRECTION, LOW);
    analogWrite(LEFT_WHEEL_SPEED, left_speed); /* 1-255 over 200 fairly unstable */
    analogWrite(RIGHT_WHEEL_SPEED, right_speed);
    delay(degree*9.78);
    analogWrite(LEFT_WHEEL_SPEED,0); /* 1-255 over 200 fairly unstable */
    analogWrite(RIGHT_WHEEL_SPEED,0);
  }
}

