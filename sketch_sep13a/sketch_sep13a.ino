int lights[5] = { A4, A3, A2, A1, A0 };
int total_lights = sizeof(lights)/sizeof(int);
int L_count = 0;
int switchMode = LOW;

void setup() {
  Serial.begin(115200);
  
}

void loop() {
  int var = analogRead(A7);

  if(var >= 1) {
    var = constrain(var, 1, 100);
    
    //Serial.println(var);
  }

  for(int i = 0; i < total_lights; i++) {
    int light_val = 0; 
    int unmapped_val = constrain(var - (20*i),0,20);
    light_val = map(unmapped_val, 0, 20, 0, 255);
    analogWrite(lights[i], light_val);
    //Serial.println(String(i)+": "+String(light_val));   
  }
    
}
