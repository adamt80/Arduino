int xPin = A7;
int xPinVal = 0;

int yPin = A6;
int yPinVal = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Connected");
}

void loop() {
  int x = analogRead(xPin);
  int y = analogRead(yPin);
  if((xPinVal <= x-2 || xPinVal >= x+2) || (yPinVal <= y-2 || yPinVal >= y+2)) {
    xPinVal = x;
    yPinVal = y;
    Serial.println("X: "+String(x)+", Y: "+String(y));
  }
  // put your main code here, to run repeatedly:

}
