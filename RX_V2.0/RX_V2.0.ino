#include <SPI.h>
#include <Mapf.h>
#include <SoftwareSerial.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Servo.h>
Servo serwo;
RF24 radio(7, 8);  // CE, CSN
const byte address[6] = "WRS01"; //musi być to samo w TX i RX, u nas każdy zestaw urządzeń będzie miał WRS01, WRS02...

#define TRIG_LED 9 //led pokazujący stan spustu 
#define STAN_LED 10 //led syg. wiele rzeczy 
#define WHEN_ON A6
#define WHEN_OFF A7
#define BATT_BUTTON 3 //przycisk od sprawdzania stanu baterii
#define BUZZER 4 //buzzer

#define VOL_METER A0 // voltage metter - sprawdza napięcię na paterii
//Ledy od poziomu baterii
#define BATT_LED1 A1
#define BATT_LED2 A2
#define BATT_LED3 A3
#define BATT_LED4 A4


int timer = 0; // podstawowe ustawienie timera odmierzającego czas trybów automatycznych
bool battCheck = false; //podstawowe ustawienia flagi informującej o tym czy jest sprawdzany stan baterii
bool isOn = false; // podstawowe ustawienie flagi informyującej czy urządzenie działa
bool deviceReady = false; // podstawowe ustawinie flagi informującej o gotowości urzadzenia
unsigned long currentTimer = 0; // aktualny czas wykorzystany przy wysyłaniu sygnału kontrolnego
unsigned long lastTimer = 0; //poprzedni czas wysłania przy jw.
unsigned long timerDiffrence = 0;// róznica pomiędzy czasem poprzednim a aktualnym
bool control = true; // flaga określająca czy sygnał kontrolny jest odbierany
byte channel = 1; // ustwa kanał na jakim ma działąć od 0 do 127
//inicjalizacja zminnych od pozycji serwa
int pozycja_on; 
int pozycja_off;

//zmienne wykorzystywane do sprawdzania baterii oraz do działania trybu auto. Zasada działania jw.
unsigned long previousMillis = 0;
const long interval = 2000;       
float voltage;

void setup(){
  Serial.begin(9600);

//Ustawienia pinmode'ów
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
  pozycja_off = map(analogRead(WHEN_OFF), 0, 1023, 0, 180);//ustalenie pozycji off
  serwo.write(pozycja_off); //ustawienie serwa w pozycji off

  //ustawianie przekaźnika
  radio.begin();
  radio.setChannel(channel); // od 1 do 127
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MIN);  //moc nadawnia sygnału
  radio.setRetries(10, 10);       //(próba wysłania co 10 * 0,25ms = 2,5ms, 10 prób)
  radio.startListening();//Włączenie nasłuchiwania na radio
}

void loop(){
  currentTimer = millis(); //ustawinie obecnego czsu dla modułu controlnego
  timerDiffrence = currentTimer - lastTimer;

  voltage = mapf(analogRead(VOL_METER), 0, 1023, 0, 5);  //pomiar napięcia na baterii
  
  while(voltage<=3.1){
    digitalWrite(BATT_LED4, HIGH);
    digitalWrite(BUZZER, HIGH);
    delay(50);
    digitalWrite(BATT_LED4, LOW);
    digitalWrite(BUZZER, LOW);
    delay(50);
  }



  
 //Ustawienia LED dla danego statusu
  if(isOn){
    digitalWrite(TRIG_LED, HIGH);
  }else{
    digitalWrite(TRIG_LED, LOW);
  }
  Serial.println(deviceReady);
  if(deviceReady){
        digitalWrite(STAN_LED, HIGH);
        
  }else{
    digitalWrite(STAN_LED, LOW);
    delay(50);
    }

  //odbieranie widomości przez system radiowy
  const char text[32] = "";
  if (radio.available()) {
    radio.read(&text, sizeof(text));
  }

  //sprawedzanie poszczególnych wiadomości
   if (String(text) == "ON5" and deviceReady == true) {
    //Serial.println("Servo 10 sekund");
    pozycja_on = map(analogRead(WHEN_ON), 0, 1023, 0, 180);
    serwo.write(pozycja_on);
              serwo.write(pozycja_on);
      isOn = true;
      timer = 10000;
    
}


   if (String(text) == "ON10" and deviceReady == true) {
   // Serial.println("Servo 20 sekund");
    pozycja_on = map(analogRead(WHEN_ON), 0, 1023, 0, 180);
    serwo.write(pozycja_on);
              serwo.write(pozycja_on);
      isOn = true;
      timer = 20000;
    
}
//System kończący tryb automatycznu w zależności od wybranej opcji (5000ms albo 10000ms)
    unsigned long currentMillis = millis();
    
    if(currentMillis - previousMillis >= timer and battCheck == false){
      previousMillis = currentMillis;
     pozycja_off = map(analogRead(WHEN_OFF), 0, 1023, 0, 180);
    serwo.write(pozycja_off);
    isOn = false;
    timer = 0;
  }


  if (String(text) == "READY" and deviceReady==false){
    deviceReady = true;
    lastTimer = currentTimer;
    for(int i = 0; i<=5; i++){ //efekt ledów oraz buzera przy parowaniu
        digitalWrite(STAN_LED, HIGH);
        digitalWrite(BUZZER, HIGH);
        delay(80);
        digitalWrite(STAN_LED, LOW);
        digitalWrite(BUZZER, LOW);
        delay(80);
        digitalWrite(STAN_LED, HIGH);
      
  }}
  if (String(text) == "ON" and deviceReady == true) {
   // Serial.println("Serwo ON");
    pozycja_on = map(analogRead(WHEN_ON), 0, 1023, 0, 180);
    serwo.write(pozycja_on);
    isOn = true;
    timer = 9999999999; // Wiem jak to wygląda.... POzwala to na przedłużenie czasu nagrywania

  }
 
  if (isOn == true and String(text) == "OFF") {
     //Serial.println("Serwo OFF");
    pozycja_off = map(analogRead(WHEN_OFF), 0, 1023, 0, 180);
    serwo.write(0);
    isOn = false;
    timer = 0;

  }

if (deviceReady == false and timerDiffrence >=100UL) {
    lastTimer = currentTimer;
    digitalWrite(STAN_LED, HIGH);
    delay(50);
}



if (String(text) == "CONTROL" and timerDiffrence >=100UL) {
    control = true;
    lastTimer = currentTimer;
    
}else if(timerDiffrence >=1000UL and deviceReady){ // określa czy urządzenie otrzymuje sygnał kontrolny. I tak wiem że można by to było zrobić bardziej efektywnie
      control = false;
      lastTimer = currentTimer;
      
    }
  

if(control == false){ // system awaryjnego wyłączenia urządzenie w przypadku braku sygnału kontrolnego po sparowaniu
  deviceReady = false;
  isOn = false;
  control = true;
  lastTimer = currentTimer;
  pozycja_off = map(analogRead(WHEN_OFF), 0, 1023, 0, 180);
  serwo.write(pozycja_off);
}
  
if(digitalRead(BATT_BUTTON)== HIGH){
  BattChceck();
}
  currentMillis = millis();
  if (currentMillis - previousMillis >= interval and battCheck) {
    previousMillis = currentMillis;
    battCheck = false;
        digitalWrite(BATT_LED1, LOW);
    digitalWrite(BATT_LED2, LOW);
    digitalWrite(BATT_LED3, LOW);
    digitalWrite(BATT_LED4, LOW);
  }
}




void BattChceck() { // to jest skopiowane więc nie wiem jak to działa. jak chcesz coś dodać to zrób to sam ;-*
  battCheck = true;
  if(digitalRead(BATT_BUTTON)==HIGH){
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

    } else if (voltage < 3.2) {  //napięcie poniżej 3,25V - 0 ledów

      digitalWrite(BATT_LED1, LOW);
      digitalWrite(BATT_LED2, LOW);
      digitalWrite(BATT_LED3, LOW);
      digitalWrite(BATT_LED4, LOW);
    }
    
    } else {

      digitalWrite(BATT_LED1, LOW);
      digitalWrite(BATT_LED2, LOW);
      digitalWrite(BATT_LED3, LOW);
      digitalWrite(BATT_LED4, LOW);
      digitalWrite(BUZZER, LOW);
    }
}
