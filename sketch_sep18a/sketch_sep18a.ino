#define Buttons       A0 

int ButtonRead = 0;
int current = 0;
int last = -1;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Connected");
}

void loop() {
    int b = 0;
    int i = 0;
    
    ButtonRead = analogRead(Buttons);
    while(ButtonRead > 50) {
       ButtonRead = analogRead(Buttons);
       b = b + ButtonRead;
       i++;
    }

    ButtonRead = b/i;
    // grab the current state of the remote-buttons
    if (ButtonRead < 120) {
      current = 0;
    }
    if (ButtonRead > 150) {
      current = 1;
    }
    if (ButtonRead > 220) {
      current = 2;
    }
    if (ButtonRead > 350) {
      current = 3;
    }
    if (ButtonRead > 500) {
      current = 4;
    }
 
  // return if the value hasn't changed
  if(current == last)
    return;

  if(current != 0 && last > 0)
    return;
    
  int32_t value = current;
  last = current;
  Serial.print(ButtonRead);
  Serial.print(": ");
  Serial.println(value);

}
