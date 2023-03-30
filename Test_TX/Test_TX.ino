#include <SPI.h>
#include <Mapf.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <JC_Button.h>

RF24 radio(7, 8);  // CE, CSN
const byte address[6] = "WRS01";
//musi być to samo w TX i RX, u nas każdy zestaw urządzeń będzie miał WRS01, WRS02...

#define TRIG_LED 9  //led pokazujący stan spustu
#define STAN_LED 10  //led syg. wiele rzeczy
#define TRIG_BUTTON 2  //przycisk od spustu
#define BATT_BUTTON 3  //przycisk od sprawdzania stanu baterii
#define VOL_METER A0
#define BATT_LED1 A1
#define BATT_LED2 A2
#define BATT_LED3 A3
#define BATT_LED4 A4
#define buzzer 

Button batt_BUTTON(3,true,70);




int ledState = LOW;                // ledState used to set the LED
unsigned long previousMillis = 0;  // will store last time LED was updated
const long interval = 2000;         // interval at which to blink (milliseconds)
float voltage;
byte timer = 0;
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
        //uint8_t numOfPresses = batt_BUTTON.multiPressRead();
        //if(numOfPresses == 1){
          //BattChceck();
        //}
    }
     Ready();
}

void BattChceck() {

  voltage = mapf(analogRead(VOL_METER), 0, 1023, 0, 5);
  Serial.println(voltage);
    if (voltage >= 4.00) {  //napięcie 4,00V i więcej - 4 ledy

      digitalWrite(BATT_LED1, HIGH);
      digitalWrite(BATT_LED2, HIGH);
      digitalWrite(BATT_LED3, HIGH);
      digitalWrite(BATT_LED4, HIGH);

    } else if (voltage < 4.00 && voltage >= 3.75) {  //napięcie od 3,75V do 4,0V - 3 ledy

      digitalWrite(BATT_LED1, LOW);
      digitalWrite(BATT_LED2, HIGH);
      digitalWrite(BATT_LED3, HIGH);
      digitalWrite(BATT_LED4, HIGH);

    } else if (voltage < 3.75 && voltage >= 3.45) {  //napięcie od 3,55V do 3,75V - 2 ledy

      digitalWrite(BATT_LED1, LOW);
      digitalWrite(BATT_LED2, LOW);
      digitalWrite(BATT_LED3, HIGH);
      digitalWrite(BATT_LED4, HIGH);

    } else if (voltage < 3.45 && voltage >= 3.25) {  //napięcie od 3,25V do 3,5V - 1 ledy

      digitalWrite(BATT_LED1, LOW);
      digitalWrite(BATT_LED2, LOW);
      digitalWrite(BATT_LED3, LOW);
      digitalWrite(BATT_LED4, HIGH);

    } else if (voltage < 3.25) {  //napięcie poniżej 3,25V - 0 ledów

      digitalWrite(BATT_LED1, LOW);
      digitalWrite(BATT_LED2, LOW);
      digitalWrite(BATT_LED3, LOW);
      digitalWrite(BATT_LED4, LOW);
 
  } else {

    digitalWrite(BATT_LED1, LOW);
    digitalWrite(BATT_LED2, LOW);
    digitalWrite(BATT_LED3, LOW);
    digitalWrite(BATT_LED4, LOW);
  }

  if (voltage < 3.25 && ledState == HIGH) {
    //buzzer
    digitalWrite(BATT_LED4, HIGH);
  }else if (voltage < 3.25 && ledState == LOW) {
    //buzzer
    digitalWrite(BATT_LED4, LOW);
  }else if (voltage >= 3.25) {
    //buzzer
  }

  //blink without delay
  
  
}



void setup() {

  Serial.begin(9600);

  pinMode(TRIG_LED, OUTPUT);
  pinMode(STAN_LED, OUTPUT);
  pinMode(BATT_LED1, OUTPUT);
  pinMode(BATT_LED2, OUTPUT);
  pinMode(BATT_LED3, OUTPUT);
  pinMode(BATT_LED4, OUTPUT);
        
  pinMode(VOL_METER, INPUT);
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
     int numOfPresses = batt_BUTTON.multiPressRead();
     if(numOfPresses !=0){
      Serial.println(numOfPresses);
     }
 if (numOfPresses!=0){
      if(numOfPresses ==5){
        Serial.println("5 razy");
        char message[] = "ON10";
        bool raport = radio.write(&message, sizeof(message));
        if (raport == true) {
        tryCounter = 0;
        isOn = true;
        timer = 10;}
      }else if(numOfPresses ==3){
    Serial.println("3 razy");
      char text[] = "ON5";
        bool raport = radio.write(&text, sizeof(text));
        if (raport == true) {
        tryCounter = 0;
        isOn = true;
        timer = 5;}
      }else if(numOfPresses==1){
      Serial.println("Stan batteri");
      BattChceck();
}            
  }
  }else{
     digitalWrite(STAN_LED, LOW);
  }

    
  
  if(digitalRead(TRIG_BUTTON)== HIGH and isOn == false and deviceReady and timer ==0){
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
  bool recive = radio.write(&text, sizeof(text));
  if(recive == 0){
    tryCounter = tryCounter + 1;   
  }else{
      tryCounter = 0;
  }
}
 if(digitalRead(TRIG_BUTTON) == HIGH and isOn and deviceReady and timer !=0){
      char text[] = "OFF";
    bool raport = radio.write(&text, sizeof(text));
    if(raport ==true){
    isOn = false;
    timer = 0;
    delay(20);
  }
  if(tryCounter>2){
    tryCounter = 0;
    isOn = false;
    deviceReady = false;
  }}
 
 if(digitalRead(TRIG_BUTTON)==LOW and isOn==true and deviceReady and timer == 0){
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

    unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
        digitalWrite(BATT_LED1, LOW);
    digitalWrite(BATT_LED2, LOW);
    digitalWrite(BATT_LED3, LOW);
    digitalWrite(BATT_LED4, LOW);
  }
  }

  
