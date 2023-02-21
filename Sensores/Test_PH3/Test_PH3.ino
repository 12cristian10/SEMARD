#define PH_PIN A0
float promVolt,temp,valores[10];

void setup() {
  Serial.begin(9600);

}

void loop() {

  for(int i=0; i<10; i++){
     valores[i] = analogRead(PH_PIN);
     delay(10);
  }
  
  for(int i = 0; i<9;  i++){

    for(int j = i+1; j<10; j++){
      if(valores[i]>valores[j]){
        temp = valores[i];
        valores[i] = valores[j];
        valores[j] = temp;
      }

    }
  }
  promVolt = 0;
  for(int i = 2; i<8;i++){
    promVolt += valores[i];
  }

  float volt = (promVolt/6)* 5.0/1024;

  float ph = (-0.018* (((volt*1000)- 1500)/3)) +6.94;

  Serial.print("Volt: ");
  Serial.println(volt);
  Serial.print("PH: ");
  Serial.println(ph);

  delay(1000);


}
