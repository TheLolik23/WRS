#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(7, 8);  // CE, CSN
const byte address[6] = "WRS01";
//musi być to samo w TX i RX, u nas każdy zestaw urządzeń będzie miał WRS01, WRS02...

#define TRIG_LED 4  //led pokazujący stan spustu
#define STAN_LED 5  //led syg. wiele rzeczy

#define TRIG_BUTTON 2  //przycisk od spustu
#define BATT_BUTTON 3  //przycisk od sprawdzania stanu baterii
bool isOn = false;
int tryCounter = 0;
bool deviceReady = false;
unsigned long aktualnyCzas = 0;
unsigned long zapamietanyCzas = 0;
unsigned long roznicaCzasu = 0;



void Ready(){
  if(deviceReady!=true){
  for(int i = 0; i<=5; i++){
        digitalWrite(STAN_LED, HIGH);
        delay(80);
        digitalWrite(STAN_LED, LOW);
        delay(80);
        deviceReady = true;
        digitalWrite(STAN_LED, HIGH);
}}
}


void start(){
  const char text[] = "READY";
  radio.write(&text, sizeof(text));
  while(radio.write(&text, sizeof(text))!= true){
        const char text[] = "READY";
        radio.write(&text, sizeof(text));
        digitalWrite(STAN_LED, LOW);
    }
     Ready();
}


void setup() {
  Serial.begin(9600);

  pinMode(TRIG_LED, OUTPUT);
  pinMode(STAN_LED, OUTPUT);

  pinMode(TRIG_BUTTON, INPUT_PULLUP);
  pinMode(BATT_BUTTON, INPUT_PULLUP);

  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MIN);  //moc nadawnia sygnału
  radio.setRetries(10, 10);
  radio.stopListening();//(próba wysłania co 10 * 0,25ms = 2,5ms, 10 prób)
  start();
}


void loop() {
  if(deviceReady == false){
    start();
  }
  
  if(digitalRead(TRIG_BUTTON)== HIGH and isOn == false){
     char text[] = "ON";
  bool raport = radio.write(&text, sizeof(text));
    Serial.println(radio.write(&text, sizeof(text)));
  if (raport == true) {
    Serial.println("ON");
    digitalWrite(TRIG_LED, HIGH);
       tryCounter = 0;
    isOn = true;
  }}
  aktualnyCzas = millis();
  roznicaCzasu = aktualnyCzas - zapamietanyCzas;
  
  
if(deviceReady and roznicaCzasu >=300UL){
  zapamietanyCzas = aktualnyCzas;
  char text[] ="CONTROL";
  Serial.println("control");
  bool recive = radio.write(&text, sizeof(text));
  if(recive == 0){
    tryCounter = tryCounter + 1;   
  }else{
      tryCounter = 0;
      digitalWrite(STAN_LED, HIGH);

  }
}

 if(digitalRead(TRIG_BUTTON)==LOW and isOn==true){
    char text[] = "OFF";
    bool raport = radio.write(&text, sizeof(text));
    if(raport ==true){
    digitalWrite(TRIG_LED, LOW);
    isOn = false;
  }}
  if(tryCounter>5){
    tryCounter = 0;
    digitalWrite(STAN_LED, LOW);
    digitalWrite(TRIG_LED, LOW);
    isOn = false;
    deviceReady = false;
  }
  }
