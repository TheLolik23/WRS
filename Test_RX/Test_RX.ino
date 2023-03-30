#include <SPI.h>
#include <Mapf.h>
#include <SoftwareSerial.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Servo.h>
Servo serwo;
RF24 radio(7, 8);  // CE, CSN
const byte address[6] = "WRS01";
//musi być to samo w TX i RX, u nas każdy zestaw urządzeń będzie miał WRS01, WRS02...

#define TRIG_LED 9 //led pokazujący stan spustu 
#define STAN_LED 10 //led syg. wiele rzeczy 
#define WHEN_ON A6
#define WHEN_OFF A7
#define BATT_BUTTON 3 //przycisk od sprawdzania stanu baterii
#define BUZZER 4

#define VOL_METER A0
#define BATT_LED1 A1
#define BATT_LED2 A2
#define BATT_LED3 A3
#define BATT_LED4 A4

bool isOn = false;
bool deviceReady = false;
unsigned long aktualnyCzas = 0;
unsigned long zapamietanyCzas = 0;
unsigned long roznicaCzasu = 0;
bool control = true;
byte channel = 1; // ustwa kanał na jakim ma działąć od 0 do 127

int pozycja_on;
int pozycja_off;


int ledState = LOW;                // ledState used to set the LED
unsigned long previousMillis = 0;  // will store last time LED was updated
const long interval = 2000;         // interval at which to blink (milliseconds)
float voltage;

void setup(){



  pinMode(TRIG_LED, OUTPUT);
  pinMode(STAN_LED, OUTPUT);
  pinMode(BATT_LED1, OUTPUT);
  pinMode(BATT_LED2, OUTPUT);
  pinMode(BATT_LED3, OUTPUT);
  pinMode(BATT_LED4, OUTPUT);
        
  pinMode(VOL_METER, INPUT);
  
  pinMode(TRIG_LED, OUTPUT);
  pinMode(STAN_LED, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(WHEN_ON, INPUT);
  pinMode(WHEN_OFF, INPUT);
  pinMode(BATT_BUTTON, INPUT_PULLUP);

  serwo.attach(5); //przypisanie pinu do serwa
  pozycja_off = map(analogRead(WHEN_OFF), 0, 1023, 0, 180);
  serwo.write(pozycja_off); //serwo 
  


  Serial.begin(9600);
  radio.begin();
  radio.setChannel(channel); // od 1 do 127
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
        digitalWrite(BUZZER, HIGH);
        delay(80);
        digitalWrite(STAN_LED, LOW);
        digitalWrite(BUZZER, LOW);
        delay(80);
        digitalWrite(STAN_LED, HIGH);
      
  }}
  if (String(text) == "ON" and deviceReady == true) {
    Serial.println("Serwo ON");
    pozycja_on = map(analogRead(WHEN_ON), 0, 1023, 0, 180);
    serwo.write(pozycja_on);
    isOn = true;

  }

  if (isOn == true and String(text) == "OFF") {
     Serial.println("Serwo OFF");
    pozycja_off = map(analogRead(WHEN_OFF), 0, 1023, 0, 180);
    serwo.write(0);
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
  
if(digitalRead(BATT_BUTTON)== HIGH){
  BattChceck();
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

void BattChceck() {
  Serial.println("test");
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
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
        digitalWrite(BATT_LED1, LOW);
    digitalWrite(BATT_LED2, LOW);
    digitalWrite(BATT_LED3, LOW);
    digitalWrite(BATT_LED4, LOW);
  }
  
  
}
