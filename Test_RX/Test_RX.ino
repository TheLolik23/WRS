#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(7, 8);  // CE, CSN
const byte address[6] = "WRS01";
//musi być to samo w TX i RX, u nas każdy zestaw urządzeń będzie miał WRS01, WRS02...

#define TRIG_LED 4 //led pokazujący stan spustu 
#define STAN_LED 5 //led syg. wiele rzeczy 

#define BATT_BUTTON 3 //przycisk od sprawdzania stanu baterii
bool isOn = false;
bool deviceReady = false;
unsigned long aktualnyCzas = 0;
unsigned long zapamietanyCzas = 0;
unsigned long roznicaCzasu = 0;
bool control = true;

void setup(){
  
  pinMode(TRIG_LED, OUTPUT);
  pinMode(STAN_LED, OUTPUT);

  pinMode(BATT_BUTTON, INPUT_PULLUP);

  Serial.begin(9600);
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MIN);  //moc nadawnia sygnału
  radio.setRetries(10, 10);       //(próba wysłania co 10 * 0,25ms = 2,5ms, 10 prób)
  radio.startListening();
}
void loop(){
   aktualnyCzas = millis();
  roznicaCzasu = aktualnyCzas - zapamietanyCzas;
  
  //LEDs
  if(isOn){
    digitalWrite(TRIG_LED, HIGH);
  }else{
    digitalWrite(TRIG_LED, LOW);
  }
  if(deviceReady){
        digitalWrite(STAN_LED, HIGH);
  }else{
     digitalWrite(STAN_LED, LOW);
  }

  
  const char text[32] = "";
  if (radio.available()) {
    radio.read(&text, sizeof(text));
  }

  if (String(text) == "READY" and deviceReady==false){
    deviceReady = true;
    zapamietanyCzas = aktualnyCzas;
    for(int i = 0; i<=5; i++){
        digitalWrite(STAN_LED, HIGH);
        delay(80);
        digitalWrite(STAN_LED, LOW);
        delay(80);
        digitalWrite(STAN_LED, HIGH);
      
  }}
  if (String(text) == "ON" and deviceReady == true) {
    isOn = true;

  }

  if (isOn == true and String(text) == "OFF") {
    isOn = false;

  }

if (String(text) == "CONTROL" and roznicaCzasu >=100UL) {
    control = true;
    zapamietanyCzas = aktualnyCzas;
    
}else if(roznicaCzasu >=1000UL and deviceReady){
      control = false;
      zapamietanyCzas = aktualnyCzas;
      
    }
  

if(control == false){
  deviceReady = false;
  isOn = false;
  control = true;
  zapamietanyCzas = aktualnyCzas;
}
  


}
