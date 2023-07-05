#define TURB_PIN 25

float analogVal = 4095, refVolt = 3.3, averageVolt = 0, ntu;
byte numRead = 100;

void setup(){
  Serial.begin(115200);
}

void loop(){
  averageVolt = 0;

  for(int i = 0; i < numRead; i++){
    averageVolt += ((float)analogRead(TURB_PIN) / analogVal) * refVolt;
  }

  averageVolt /= numRead;

  if(averageVolt < 2.5 ){
    ntu = 3000;
  }else if(averageVolt > 4.2){
    ntu = 0;
  }else{
    ntu = -1120.4 * sq(averageVolt) + 5742.3 * averageVolt - 4352.9;
  }


  Serial.print("Voltaje: ");
  Serial.println(averageVolt);
  Serial.print("NTU: ");
  Serial.println(ntu);
  delay(1000);

}