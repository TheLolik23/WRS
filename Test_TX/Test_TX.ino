#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <PinButton.h>

RF24 radio(7, 8);  // CE, CSN
const byte address[6] = "WRS01";
//musi być to samo w TX i RX, u nas każdy zestaw urządzeń będzie miał WRS01, WRS02...

#define TRIG_LED 9  //led pokazujący stan spustu
#define STAN_LED 10  //led syg. wiele rzeczy
#define TRIG_BUTTON 2  //przycisk od spustu
#define BATT_BUTTON 3  //przycisk od sprawdzania stanu baterii
PinButton batt_BUTTON(3);
bool isOn = false;
int tryCounter = 0;
bool deviceReady = false;
unsigned long aktualnyCzas = 0;
unsigned long zapamietanyCzas = 0;
unsigned long roznicaCzasu = 0;
byte channel = 1;// ustwa kanał na jakim ma działąć od 0 do 127



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
  radio.setChannel(channel); // od 1 do 127
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MIN);  //moc nadawnia sygnału
  radio.setRetries(10, 10);
  radio.stopListening();//(próba wysłania co 10 * 0,25ms = 2,5ms, 10 prób)
  start();
}


void loop() {
  batt_BUTTON.update();
  if(deviceReady == false){
    start();
  }
    
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
if (batt_BUTTON.isDoubleClick() and deviceReady) {
    Serial.println("Button was pressed twice!");
  }
  
  if(digitalRead(TRIG_BUTTON)== HIGH and isOn == false and deviceReady){
     char text[] = "ON";
  bool raport = radio.write(&text, sizeof(text));
  if (raport == true) {
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
  }
}

 if(digitalRead(TRIG_BUTTON)==LOW and isOn==true and deviceReady){
    char text[] = "OFF";
    bool raport = radio.write(&text, sizeof(text));
    if(raport ==true){
    isOn = false;
  }}
  if(tryCounter>2){
    tryCounter = 0;
    isOn = false;
    deviceReady = false;
  }
  }
