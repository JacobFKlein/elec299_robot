#include <Servo.h>

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
#define LTHRESH 750
#define CTHRESH 750
#define RTHRESH 750

//bumper pins
#define LEFT_BUMPER 11
#define RIGHT_BUMPER 13

//wheel speeds that allow robot to drive straight
byte left_speed = 126;
byte right_speed = 114; 

//global bluetooth input variable
char BTin;

/* current position variable
  values(Left,Center,Right,Front,Back) */
String position = "Center";
/* next position variable
  values(Left,Center, Right,Front,Back) */
String nextPosition;

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

//used to initialize all pins and servos in the robot
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
  tilt.write(120);
  pan.attach(PAN);
  pan.write(90);
  grab.attach(GRIP);
  grab.write(80);
 }

/* this function waits until a bumper is pressed
   (similar to having a pushbutton) */
void WaitBumper() {
  while (digitalRead(LEFT_BUMPER) == LOW &&
    digitalRead(RIGHT_BUMPER) == LOW) {}
  delay(250);
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
  analogWrite(LEFT_WHEEL_SPEED, left_speed); /* 1-255 over 200 fairly unstable */
  analogWrite(RIGHT_WHEEL_SPEED, right_speed);
  delay(degree*9.78);
  analogWrite(LEFT_WHEEL_SPEED,0); /* 1-255 over 200 fairly unstable */
  analogWrite(RIGHT_WHEEL_SPEED,0);
}

/* using light sensors allows the robot to follow a
   black line until it hits a wall */
void FollowLine() { /* Follows line until hits wall */
  while (digitalRead(LEFT_BUMPER) == LOW || digitalRead(RIGHT_BUMPER) == LOW) {
    digitalWrite(LEFT_WHEEL_DIRECTION, HIGH);
    digitalWrite(RIGHT_WHEEL_DIRECTION, HIGH);
    if (analogRead(CenterLightSensor) < CTHRESH) {
      analogWrite(LEFT_WHEEL_SPEED, left_speed); /* 1-255 over 200 fairly unstable */
      analogWrite(RIGHT_WHEEL_SPEED, right_speed);
      delay(50);
    }
    else if (analogRead(LeftLightSensor) < LTHRESH) {
      analogWrite(LEFT_WHEEL_SPEED, left_speed-30); /* 1-255 over 200 fairly unstable */
      analogWrite(RIGHT_WHEEL_SPEED, right_speed+30);
      delay(50);
    }
    else if (analogRead(RightLightSensor) < RTHRESH) {
      analogWrite(LEFT_WHEEL_SPEED, left_speed+30); /* 1-255 over 200 fairly unstable */
      analogWrite(RIGHT_WHEEL_SPEED, right_speed-30);
      delay(50);
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
  delay(450);
  Backup(1000);
}

//INCOMPLETE
/* allows the robot to recieve the location of the next ball
   through either Infrared or Bluetooth */
void getNextLocation() {
	while (true) {
	//turn 45
	//search ir
	//if get a 1/2/3 from ir
		//finish 360 turn
		//set nextlocation to 1/2/3
		//break;
	//search BT
	//if get a 1/2/3 from BT
		//finish 360 turn
		//set nextlocation to 1/2/3
		//break;
	}

}

