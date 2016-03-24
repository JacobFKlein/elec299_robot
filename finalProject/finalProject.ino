#include <Servo.h>
#include <QSerial.h>

//servo pins and variables
#define TILT 9
#define PAN 8
#define GRIP 10
#define PRESSURE A1 //NEED TO MOVE TO ANOTHER ANALOG PIN
Servo tilt, pan, grab;

//wheel pins
#define LEFT_WHEEL_DIRECTION 4
#define LEFT_WHEEL_SPEED 5
#define RIGHT_WHEEL_SPEED 6
#define RIGHT_WHEEL_DIRECTION 7

//light sensor pins and thresholds
#define LeftLightSensor A5
#define CenterLightSensor A4
#define RightLightSensor A3
#define LTHRESH 500
#define CTHRESH 500
#define RTHRESH 500 // was 650 for all 

//bumper pins
#define LEFT_BUMPER 11
#define RIGHT_BUMPER 13

//wheel speeds that allow robot to drive straight
byte left_speed = 126;
byte right_speed = 114; 

//global bluetooth input variable
char BTin;
char BTin2;
char Target;
/* current position variable
  values(Left,Center,Right,Front,Back) */
String position = "Center";
/* next position variable
  values(Left,Center, Right,Front,Back) */
String nextPosition;

QSerial myIRserial;
void setup() {
  Initialize();
  Serial.begin(115200);
  WaitBumper();
}

void loop() {
  //BTin = GetBT();
  //Initialize();
  delay(500);
  Target = getNextTarget();
  //SPIN(90, 'L');
  FollowLine();
  PickupBall();
  DepositBall();
  /*
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
  */
}

//used to initialize all pins and servos in the robot
void Initialize() {
  /* Declaring inputs and outputs */
  pinMode(LeftLightSensor, INPUT);
  pinMode(CenterLightSensor, INPUT);
  pinMode(RightLightSensor, INPUT);
  pinMode(LEFT_WHEEL_SPEED, OUTPUT);
  pinMode(LEFT_WHEEL_DIRECTION, OUTPUT);
  pinMode(RIGHT_WHEEL_SPEED, OUTPUT);
  pinMode(RIGHT_WHEEL_DIRECTION, OUTPUT);
  pinMode(LEFT_BUMPER, INPUT);
  pinMode(RIGHT_BUMPER, INPUT);
  pinMode(LeftLightSensor, INPUT);
  pinMode(CenterLightSensor, INPUT);
  pinMode(RightLightSensor, INPUT);

  myIRserial.attach(A0, -1); //-1 in second column means recieve
  
  /* Initializing Servos (so they dont shake) */
  tilt.attach(TILT);
  tilt.write(120);
  pan.attach(PAN);
  pan.write(90);
  grab.attach(GRIP);
  grab.write(80);
 }

/* this function waits until a bumper is pressed
   (similar to having a pushbutton) */
void WaitBumper() {
/*  while (digitalRead(LEFT_BUMPER) == LOW &&
    digitalRead(RIGHT_BUMPER) == LOW) {}
  delay(250);*/

  while (true) {
    if (Serial.available()) {
      BTin2 = Serial.read();
      if (BTin2 == '1' || BTin2 == '0' || BTin2 == '2') {
        break;
      }
    }
  }
}
//gets a character response from the BT module
char GetBT() {
  BTin = ' ';
  if (Serial.available()) {
    BTin = Serial.read();
  }
  return BTin;
}

//spins the robot a given degree in a given direction
void SPIN(int degree, char direction) {
  if (direction == 'L') {
    digitalWrite(LEFT_WHEEL_DIRECTION, LOW);
    digitalWrite(RIGHT_WHEEL_DIRECTION, HIGH);
  }
  else if (direction == 'R') {
    digitalWrite(LEFT_WHEEL_DIRECTION, HIGH);
    digitalWrite(RIGHT_WHEEL_DIRECTION, LOW);
  }

  analogWrite(LEFT_WHEEL_SPEED, .8*left_speed); /* 1-255 over 200 fairly unstable */
  analogWrite(RIGHT_WHEEL_SPEED, .8*right_speed);
  delay(600);
  
  while (analogRead(LeftLightSensor) >= LTHRESH  || analogRead(RightLightSensor) >= RTHRESH 
      || analogRead(CenterLightSensor) >= CTHRESH) {}    

  if (degree > 90) {
    SPIN(degree-90, direction);
  }
  else {
  analogWrite(LEFT_WHEEL_SPEED,0); /* 1-255 over 200 fairly unstable */
  analogWrite(RIGHT_WHEEL_SPEED,0);
  }
}

/* using light sensors allows the robot to follow a
   black line until it hits a wall */
void FollowLine() { /* Follows line until hits wall */
  while (digitalRead(LEFT_BUMPER) == LOW && digitalRead(RIGHT_BUMPER) == LOW) {
    digitalWrite(LEFT_WHEEL_DIRECTION, HIGH);
    digitalWrite(RIGHT_WHEEL_DIRECTION, HIGH);
    if (analogRead(CenterLightSensor) < CTHRESH) {
      analogWrite(LEFT_WHEEL_SPEED, left_speed); /* 1-255 over 200 fairly unstable */
      analogWrite(RIGHT_WHEEL_SPEED, right_speed);
    }
    if (analogRead(LeftLightSensor) < LTHRESH) {
      analogWrite(LEFT_WHEEL_SPEED, left_speed-30); /* 1-255 over 200 fairly unstable */
      analogWrite(RIGHT_WHEEL_SPEED, right_speed+30);
    }
    if (analogRead(RightLightSensor) < RTHRESH) {
      analogWrite(LEFT_WHEEL_SPEED, left_speed+30); /* 1-255 over 200 fairly unstable */
      analogWrite(RIGHT_WHEEL_SPEED, right_speed-30);
    }
  }
}

//makes the robot backup for a given time
void Backup(int timeMsec) {
  digitalWrite(LEFT_WHEEL_DIRECTION, LOW);
  digitalWrite(RIGHT_WHEEL_DIRECTION, LOW);
  analogWrite(LEFT_WHEEL_SPEED, left_speed);
  analogWrite(RIGHT_WHEEL_SPEED, right_speed);
  delay(timeMsec);
  analogWrite(LEFT_WHEEL_SPEED, 0);
  analogWrite(RIGHT_WHEEL_SPEED, 0);
  
}

//makes the robot pickup the ball
void PickupBall() {
  Backup(165);
  tilt.write(40);
  pan.write(90);
  delay(400);
  grab.write(110);
  delay(250);
  tilt.write(120);
  delay(250);
  Backup(1000);
}

//INCOMPLETE
/* allows the robot to recieve the location of the next ball
   through either Infrared or Bluetooth */
char getNextTarget() {
  int spins = 0;
  char temp = ' ';
  while (true) {
    for (int i = 0; i < 15; i++) {
      temp = getIRBTSignal();
      if (temp == '0' || temp == '1' || temp == '2') {
        delay(400);
        break;
      }
     if (BTin2 != -1) {
        temp = BTin2;
        BTin2 = -1;
        delay(400);
        break;
      }
    }
    if (temp == '1') {
      while (spins%4!=0) {
        SPIN(90, 'L');
        delay(350);
        spins++;
      }
      return '1';
      break;
    }
    else if (temp == '2') {
      while (spins%4 !=1) {
        SPIN(90, 'L');
        spins++;
        delay(350);
      }
      return '2';
      break;
    }
    else if (temp == '0') {
      while (spins%4!=3) {
        SPIN(90, 'L');
        spins++;
        delay(350);
      }
      return '0';
      break;
    }
    else {
      SPIN(90, 'L');
      spins++;
      delay(200);
    }
  }
}

char getIRBTSignal() {
  //char val = myIRserial.receive(200);
  //delay(200);

  BTin = ' ';
  long timer = millis();
  while (millis() - timer < 200) {
    if (Serial.available()) {
      BTin = Serial.read();
      if (BTin != ' ') {
        return BTin;
      }
    }
  }
  return BTin;
}

void DepositBall() {
  SPIN(90, 'L');
  delay(200);
    if (Target ==  '0') {
    GoToCenter();
    SPIN(90, 'L');
    delay(200);
  }
  else if (Target == '1') {
    SPIN(90, 'L');
    delay(200);
  }
  else if (Target == '2') {
    GoToCenter();
    SPIN(90, 'R');
     delay(200);
  }
  FollowLine();
  Dunk();
  Backup(1000);
  SPIN(90, 'L');
  delay(300);
  GoToCenter();
  delay(450);
  
}

void GoToCenter() {
    while (analogRead(LeftLightSensor) >= LTHRESH  || analogRead(RightLightSensor) >= RTHRESH 
      || analogRead(CenterLightSensor) >= CTHRESH){
      digitalWrite(LEFT_WHEEL_DIRECTION, HIGH);
      digitalWrite(RIGHT_WHEEL_DIRECTION, HIGH);
      if (analogRead(CenterLightSensor) < CTHRESH) {
        analogWrite(LEFT_WHEEL_SPEED, left_speed); /* 1-255 over 200 fairly unstable */
        analogWrite(RIGHT_WHEEL_SPEED, right_speed);
      }
      if (analogRead(LeftLightSensor) < LTHRESH) {
        analogWrite(LEFT_WHEEL_SPEED, left_speed-30); /* 1-255 over 200 fairly unstable */
        analogWrite(RIGHT_WHEEL_SPEED, right_speed+30);
      }
      if (analogRead(RightLightSensor) < RTHRESH) {
        analogWrite(LEFT_WHEEL_SPEED, left_speed+30); /* 1-255 over 200 fairly unstable */
        analogWrite(RIGHT_WHEEL_SPEED, right_speed-30);
      }
  }
  delay(50);
}

void Dunk() {
  tilt.write(100);
  delay(700);
  pan.write(90);
  delay(300);
  grab.write(100);
  delay(200);
  grab.write(90);
  delay(200);
  grab.write(80);
  delay(200);


}



