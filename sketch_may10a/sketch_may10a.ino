void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  //Wait for serial connection
  while(! Serial);
  Serial.println("Connected!");
}

void loop() {
  // put your main code here, to run repeatedly:

}
