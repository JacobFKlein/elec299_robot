#include <Servo.h>
#define TILT 9
#define PAN 8
#define GRIP 10
#define PRESSURE A1 //NEED TO MOVE TO ANOTHER ANALOG PIN

#define LEFT_WHEEL_DIRECTION 4
#define LEFT_WHEEL_SPEED 5
#define RIGHT_WHEEL_SPEED 6
#define RIGHT_WHEEL_DIRECTION 7

#define IRL A5
#define IRC A4
#define IRR A3
#define LTHRESH 280
#define CTHRESH 280
#define RTHRESH 280

#define LEFT_BUMPER 11
#define RIGHT_BUMPER 13

byte left_speed = 126;
byte right_speed = 114; //PWM values from 0 to 255
Servo tilt, pan, grab;
char BTin;

void setup() {
  Initialize();
  WaitBumper();
  Serial.begin(115200);
}

void loop() {
  //BTin = GetBT();
  delay(2500);
  SPIN(90, 'L');
  switch (BTin) {
    case '1': // go to left one spin, left 90
      SPIN(90, 'L');
      FollowLine();
      break;
    case '2': // go to right one spin, right 90
      SPIN(90,'R');
      FollowLine();
      break;
    case '3': // go to back one, spin 180
      SPIN(180, 'L');
      FollowLine();
      break;
    default: //otherwise do...
      break;
  }
}

void Initialize() {
  /* Declaring inputs and outputs */
  pinMode(LEFT_WHEEL_SPEED, INPUT);
  pinMode(LEFT_WHEEL_DIRECTION, INPUT);
  pinMode(RIGHT_WHEEL_SPEED, INPUT);
  pinMode(RIGHT_WHEEL_DIRECTION, INPUT);
  pinMode(LEFT_BUMPER, INPUT);
  pinMode(RIGHT_BUMPER, INPUT);
  pinMode(IRL, OUTPUT);
  pinMode(IRC, OUTPUT);
  pinMode(IRR, OUTPUT);
  
  /* Initializing Servos (so they dont shake) */
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
  delay(250);
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
  }
  else if (direction == 'R') {
    digitalWrite(LEFT_WHEEL_DIRECTION, HIGH);
    digitalWrite(RIGHT_WHEEL_DIRECTION, LOW);
  }
  analogWrite(LEFT_WHEEL_SPEED, left_speed); /* 1-255 over 200 fairly unstable */
  analogWrite(RIGHT_WHEEL_SPEED, right_speed);
  delay(degree*9.78);
  analogWrite(LEFT_WHEEL_SPEED,0); /* 1-255 over 200 fairly unstable */
  analogWrite(RIGHT_WHEEL_SPEED,0);
}

void FollowLine() { /* Follows line until hits wall */
  while (digitalRead(LEFT_BUMPER) == LOW || digitalRead(RIGHT_BUMPER) == LOW) {
    digitalWrite(LEFT_WHEEL_DIRECTION, HIGH);
    digitalWrite(RIGHT_WHEEL_DIRECTION, HIGH);
    while (CenterSensor < CTHRESH) {
      analogWrite(LEFT_WHEEL_SPEED, left_speed); /* 1-255 over 200 fairly unstable */
      analogWrite(RIGHT_WHEEL_SPEED, right_speed);
    }
    if (LeftSensor < LTHRESH) {
      analogWrite(LEFT_WHEEL_SPEED, left_speed-10); /* 1-255 over 200 fairly unstable */
      analogWrite(RIGHT_WHEEL_SPEED, right_speed+10);
    }
      if (RightSensor < RTHRESH) {
        analogWrite(LEFT_WHEEL_SPEED, left_speed+10); /* 1-255 over 200 fairly unstable */
        analogWrite(RIGHT_WHEEL_SPEED, right_speed-10);
    }
  }
}

