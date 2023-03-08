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
int counter = 0;
bool deviceReady = false;

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
  const char text[32] = "";
  if (radio.available()) {
    radio.read(&text, sizeof(text));
    Serial.println("radio.read(&text, sizeof(text))");
  }

  if (String(text) == "READY" and deviceReady==false){
    deviceReady = true;
    for(int i = 0; i<=5; i++){
        digitalWrite(STAN_LED, HIGH);
        delay(80);
        digitalWrite(STAN_LED, LOW);
        delay(80);
        digitalWrite(STAN_LED, HIGH);
      
  }}
  if (String(text) == "ON" and deviceReady == true) {
    Serial.println("Działa");
    isOn = true;
    digitalWrite(TRIG_LED, HIGH);
    counter = 0;

  }

  if (isOn == true and String(text) == "OFF") {
    Serial.println("OFF");
    isOn = false;
    digitalWrite(TRIG_LED, LOW);
    counter = 0;

  }

if (isOn and String(text) != "CONTROL") {
    counter = counter + 1;
  
}
if (counter == 10) {
    isOn = false;
    digitalWrite(TRIG_LED, LOW);
    digitalWrite(STAN_LED, LOW);
    counter = 0;
  }


}
