class MultiPressButton {
  public:
    MultiPressButton(int pin);
    bool wasPressed(int times);
  private:
    int _pin;
    int _prevState;
    unsigned long _lastPressTime;
};

MultiPressButton::MultiPressButton(int pin) {
  pinMode(pin, INPUT_PULLUP);
  _pin = pin;
  _prevState = HIGH;
  _lastPressTime = 0;
}

bool MultiPressButton::wasPressed(int times) {
  bool result = false;
  int currentState = digitalRead(_pin);
  
  if (currentState != _prevState) {
    if (currentState == LOW) {
      unsigned long currentTime = millis();
      if (currentTime - _lastPressTime > 50) {
        _lastPressTime = currentTime;
        result = true;
      }
    }
    _prevState = currentState;
  }
  
  if (result) {
    for (int i = 1; i < times; i++) {
      delay(50);
      currentState = digitalRead(_pin);
      if (currentState != _prevState) {
        _prevState = currentState;
        result = false;
        break;
      }
    }
  }
  
  return result;
}
