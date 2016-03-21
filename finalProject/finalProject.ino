#include <Servo.h>
#define TILT 9
#define PAN 8
#define GRIP 10
#define PRESSURE A1 //NEED TO MOVE TO ANOTHER ANALOG PIN

#define LEFT_WHEEL_DIRECTION 4
#define LEFT_WHEEL_SPEED 5
#define RIGHT_WHEEL_SPEED 6
#define RIGHT_WHEEL_DIRECTION 7

#define LeftLightSensor A5
#define CenterLightSensor A4
#define RightLightSensor A3
#define LTHRESH 750
#define CTHRESH 750
#define RTHRESH 750

#define LEFT_BUMPER 11
#define RIGHT_BUMPER 13

byte left_speed = 126;
byte right_speed = 114; //PWM values from 0 to 255
Servo tilt, pan, grab;
char BTin;

void setup() {
  Initialize();
  WaitBumper();
 // Serial.begin(115200);
}

void loop() {
  //BTin = GetBT();
  Initialize();
  delay(2500);
  SPIN(90, 'L');
  FollowLine();
  PickupBall();
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
  pinMode(LEFT_WHEEL_SPEED, OUTPUT);
  pinMode(LEFT_WHEEL_DIRECTION, OUTPUT);
  pinMode(RIGHT_WHEEL_SPEED, OUTPUT);
  pinMode(RIGHT_WHEEL_DIRECTION, OUTPUT);
  pinMode(LEFT_BUMPER, INPUT);
  pinMode(RIGHT_BUMPER, INPUT);
  
  pinMode(LeftLightSensor, INPUT);
  pinMode(CenterLightSensor, INPUT);
  pinMode(RightLightSensor, INPUT);
  
  /* Initializing Servos (so they dont shake) */
  tilt.attach(TILT);
  // tilt.write(48); FOR GRABBALL
  tilt.write(120);
  pan.attach(PAN);
  pan.write(90);
  grab.attach(GRIP);
  grab.write(80);
 }

void WaitBumper() {
  while (digitalRead(LEFT_BUMPER) == LOW &&
    digitalRead(RIGHT_BUMPER) == LOW) {}
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
    if (digitalRead(CenterLightSensor) < CTHRESH) {
      analogWrite(LEFT_WHEEL_SPEED, left_speed); /* 1-255 over 200 fairly unstable */
      analogWrite(RIGHT_WHEEL_SPEED, right_speed);
      delay(50);
    }
    else if (digitalRead(LeftLightSensor) < LTHRESH) {
      analogWrite(LEFT_WHEEL_SPEED, left_speed-80); /* 1-255 over 200 fairly unstable */
      analogWrite(RIGHT_WHEEL_SPEED, right_speed+80);
      delay(50);
    }
    else if (digitalRead(RightLightSensor) < RTHRESH) {
      analogWrite(LEFT_WHEEL_SPEED, left_speed+80); /* 1-255 over 200 fairly unstable */
      analogWrite(RIGHT_WHEEL_SPEED, right_speed-80);
      delay(50);
    }
  }
}

void Backup(int timeMsec) {
  digitalWrite(LEFT_WHEEL_DIRECTION, LOW);
  digitalWrite(RIGHT_WHEEL_DIRECTION, LOW);
  analogWrite(LEFT_WHEEL_SPEED, left_speed);
  analogWrite(RIGHT_WHEEL_SPEED, right_speed);
  delay(timeMsec);
  analogWrite(LEFT_WHEEL_SPEED, 0);
  analogWrite(RIGHT_WHEEL_SPEED, 0);
  
}

void PickupBall() {
  Backup(165);
  tilt.write(40);
  pan.write(90);
  delay(400);
  grab.write(110);
  delay(450);
  Backup(1000);
}

