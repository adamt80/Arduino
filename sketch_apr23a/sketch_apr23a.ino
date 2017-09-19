//#include <Servo.h>

//Ultrasonic Sensor
#define trigPin 12
#define echoPin 13
//Right Wheel
#define RightA 2
#define RightB 3
#define RightPwr 4
//Left Wheel
#define LeftA 5
#define LeftB 6
#define LeftPwr 7

//Auto vs Manual control
int manual = 0;
int auto_timer = 0;
int auto_delay = 15;

//Direction it's moving (Stop, Fwd, Back, Right, Left)
int dir = 0;

//Ping info
int long duration, distance;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  //Wait for serial connection
  //while(! Serial);
  pinMode(RightPwr, OUTPUT);
  pinMode(LeftPwr, OUTPUT);
  pinMode(RightA, OUTPUT);
  pinMode(RightB, OUTPUT);
  pinMode(LeftA, OUTPUT);
  pinMode(LeftB, OUTPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  Serial.println("Connected!");
}

void fullStop() {
    dir = 0;
    analogWrite(LeftPwr, 0);
    analogWrite(RightPwr, 0);
    digitalWrite(LeftA, LOW);
    digitalWrite(LeftB, LOW);
    digitalWrite(RightA, LOW);
    digitalWrite(RightB, LOW);
    delay(300);
}

void doForward() {
    if(dir == 1)
      return;
    dir = 1;
    digitalWrite(LeftA, HIGH);
    digitalWrite(LeftB, LOW);
    digitalWrite(RightA, HIGH);
    digitalWrite(RightB, LOW);
    analogWrite(LeftPwr, 150);
    analogWrite(RightPwr, 150);
}

void doReverse() {
    if(dir == 2)
      return;
    dir = 2;
    digitalWrite(LeftA, LOW);
    digitalWrite(LeftB, HIGH);
    digitalWrite(RightA, LOW);
    digitalWrite(RightB, HIGH);
    analogWrite(LeftPwr, 150);
    analogWrite(RightPwr, 150);
}

void doRight() {
    if(dir == 3)
      return;
    dir = 3;
    digitalWrite(LeftA, LOW);
    digitalWrite(LeftB, LOW);
    digitalWrite(RightA, LOW);
    digitalWrite(RightB, HIGH);
    analogWrite(LeftPwr, 0);
    analogWrite(RightPwr, 150);
}

void doLeft() {
    if(dir == 1)
      return;
    dir = 4;
    digitalWrite(LeftA, LOW);
    digitalWrite(LeftB, HIGH);
    digitalWrite(RightA, LOW);
    digitalWrite(RightB, LOW);
    analogWrite(LeftPwr, 150);
    analogWrite(RightPwr, 0);
}

void checkInput() {
    if ( Serial.available()) {
    char c = Serial.read();
    switch(c) {
     case 'w':
      doForward();
     break;
     case 's':
      doReverse();
     break;
     case 'a':
      doLeft();
     break;
     case 'd':
      doRight();
     break;
     case ' ':
      fullStop();
      Serial.println("Stop");
     break;
    }
    if(c != ' ')
      Serial.println(c);
    auto_timer = 0;
  } else {
    if(manual)
     auto_timer++;
    if(auto_timer > (auto_delay * 2)) {
     manual = 0;
    }
  }
}

void doPing() {
  digitalWrite(trigPin, LOW);  // Added this line
  delayMicroseconds(2); // Added this line
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10); // Added this line
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = (duration/2) / 29.1;
  
  if (Serial && distance < 0){
    Serial.println("Out of range");
  } else if(Serial) {
    Serial.print(distance);
    Serial.println(" cm");
  }
  
}

void loop() {
  // put your main code here, to run repeatedly:
  doPing();  //Ping for obstacles with ultrasonic range finder and set distance.
  checkInput(); //Check for serial commands and toggle between automatic and manual control
  if(distance < 15 && ! manual) {
    fullStop();
    doLeft();
    delay(300);
    fullStop();
  } else if(! manual) {
    doForward();
  }
  delay(500);
}
