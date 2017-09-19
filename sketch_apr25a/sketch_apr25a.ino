#include <Servo.h>

Servo myServo;
int pos = 0;

//Ultrasonic Sensor
#define trigPin 12
#define echoPin 13

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
  while(! Serial);
  myServo.attach(9);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  Serial.println("Connected!");
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
  for(pos = 0; pos < 180; pos += 1) // goes from 0 degrees to 180 degrees 
  { // in steps of 1 degree 
    myServo.write(pos); // tell servo to go to position in variable 'pos' 
    doPing();
    delay(15); // waits 15ms for the servo to reach the position 
  } 
  for(pos = 180; pos>=1; pos-=1) // goes from 180 degrees to 0 degrees 
  { 
    myServo.write(pos); // tell servo to go to position in variable 'pos'
    doPing(); 
    delay(15); // waits 15ms for the servo to reach the position 
  } 
}
