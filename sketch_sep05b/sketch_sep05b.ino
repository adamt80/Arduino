// Declare the pins for the Button and the LED
int buttonPin = 3;
int LED = 13;

int buttonState = 0;

void setup() {
   // Define pin #12 as input and activate the internal pull-up resistor
   pinMode(buttonPin, INPUT_PULLUP);
   // Define pin #13 as output, for the LED
   pinMode(LED, OUTPUT);
}


void do_buttonDown() {

}

void do_buttonUp() {
 
}

void do_buttonHold() {
  
}

void do_buttonClick() {
  if(digitalRead(LED) != HIGH)
     digitalWrite(LED,HIGH);
  else
     digitalWrite(LED,LOW);
}

void loop(){
   // Read the value of the input. It can either be 1 or 0
   int buttonValue = digitalRead(buttonPin);
   if (buttonValue == LOW){
      if(buttonState != 1)
          do_buttonDown();
      else
          do_buttonHold();    
      buttonState = 1;   
   } else {
      if(buttonState != 0)
        do_buttonUp();
      if(buttonState == 1)
        do_buttonClick();
      buttonState = 0;
   }
}
