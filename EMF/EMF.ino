int lights[5] = {A3, A2, A1 };
int total_lights = sizeof(lights)/sizeof(int);
int L_count = 0;
int switchMode = LOW;

void setup() {
  Serial.begin(115200);
  
}

void loop() {
  int var = analogRead(A0);

  if(var >= 1) {
    var = constrain(var, 1, 100);
    
    //Serial.println(var);
  }
  int lmax = 100/total_lights;
  for(int i = 0; i < total_lights; i++) {
    int light_val = 0; 
    
    int unmapped_val = constrain(var - (lmax*i),0,lmax);
    light_val = map(unmapped_val, 0, lmax, 0, 255);
    analogWrite(lights[i], light_val);
    //Serial.println(String(i)+": "+String(light_val));   
  }
    
}
