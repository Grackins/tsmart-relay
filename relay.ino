class Relay {
private:
  int pin;
  bool closed;

public:
  Relay(int _pin) {
    this->pin = _pin;
    this->closed = false;
    pinMode(_pin, OUTPUT);
  }

  void setState(bool state) {
    this->closed = state;
    digitalWrite(this->pin, state ? HIGH : LOW);
  }

  bool getState() {
    return this->closed;
  }
};

class Sensor {
private:
  int pin;
  bool lastState;

public:
  Sensor(int _pin) {
    this->pin = _pin;
    this->lastState = false;
    pinMode(_pin, INPUT);
  }

  bool getState() {
    bool currentState = digitalRead(this->pin) == HIGH;
    bool result = currentState and !lastState;
    lastState = currentState;
    return result;
  }
};

class RelayController {
private:
  Sensor *sensor;
  Relay *relay;

public:
  RelayController(int sensorPin, int relayPin) {
    sensor = new Sensor(sensorPin);
    relay = new Relay(relayPin);
  }

  void stepForward() {
    relay->setState(sensor->getState() ^ relay->getState());
  }

  bool getState() {
    return relay->getState();
  }
};

RelayController *controller;

void setup() {
  controller = new RelayController(16, 5);
}

void loop() {
  controller->stepForward();
  delay(100);
}
