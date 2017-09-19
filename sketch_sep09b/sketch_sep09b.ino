int pin = 3;

bool fadeMode = true;
int brightness = 0;
int fadeAmount = 5;

void setup() {
    Serial.begin(9600);

    while(!Serial) {

      ;
    }
    Serial.println("Serial Connected");
    
    pinMode(pin, OUTPUT);
    pinMode(13, OUTPUT);
    analogWrite(pin, 255);
    analogWrite(13, 255);
    
}

void loop()
{
    if(Serial.available()) {
      char r = Serial.read();
      String c = String(r);
      
       if(fadeMode == true) { 
          fadeMode = false;
          Serial.println("Fade Mode -> Off");
       }
       if(c == "F" && fadeMode == false) {
          fadeMode = true;
          Serial.println("Fade Mode -> On");
       }

       if(c == "0") {
          analogWrite(pin, 0);
          analogWrite(13, 0);
    
          Serial.println("Pin -> Off");
       }
       if(c == "1") {
          analogWrite(pin, 255);
          analogWrite(13, 255);
    
          Serial.println("Pin -> On");
       }
    }
    if(fadeMode) {
          brightness = brightness + fadeAmount;

          // reverse the direction of the fading at the ends of the fade:
          if (brightness <= 0 || brightness >= 255) {
            fadeAmount = -fadeAmount;
          }
          analogWrite(pin, brightness);
          // wait for 30 milliseconds to see the dimming effect
          delay(30);
    }
        

}




