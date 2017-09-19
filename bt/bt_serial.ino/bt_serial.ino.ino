/*
 * Bluetooth Example
 */
#include <SoftwareSerial.h>

SoftwareSerial mySerial(10, 11); // RX, TX
int i = 0;
String buff = "";

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }


  Serial.println("Serial Connected!");

  // set the data rate for the SoftwareSerial port
  mySerial.begin(9600);
  mySerial.println("Bluetooth Connected!");
}

String buttons[10] {
"START",
"SQUARE",
"TRIANGLE",
"X",
"O",
"UP",
"DOWN",
"LEFT",
"RIGHT",
"SELECT"  
};

void processCommand(String b) {
  String button = String(b.charAt(2));
  Serial.println("Pressed button "+buttons[button.toInt()]);
}

void loop() { // run over and over
  
  if (mySerial.available()) {
    char b = mySerial.read();
    int blen = buff.length();
    String c = String(b);
    if((c == "!" && blen == 0) || (blen > 0 && blen < 4)) {
      buff = buff + c;          
    }
    if(buff.length() == 4) {
      processCommand(buff);
      buff = "";
    } else if(buff.length() == 0) {
      Serial.write(b);
    }
    
  }
  if (Serial.available()) {
    mySerial.write(Serial.read());
  } /*else {
    mySerial.println(i);
    delay(1000);
    i=i+1;   
  }*/
}

