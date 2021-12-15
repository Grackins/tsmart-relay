#include <ESP8266WiFi.h>

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
  WiFiServer *server;

public:
  RelayController(int sensorPin, int relayPin, int port) {
    sensor = new Sensor(sensorPin);
    relay = new Relay(relayPin);

    server = new WiFiServer(port);
    server->begin();
    server->setNoDelay(true);
  }

  ~RelayController() {
    server->close();
    delete server;
    delete sensor;
    delete relay;
  }

  void stepForward() {
    relay->setState(sensor->getState() ^ relay->getState());

    WiFiClient client = server->available();
    if (!client)
      return;
    while (client.connected()) {
      if (client.available()) {
        String line = client.readStringUntil('\n');
        if (line[1] == '0') {
          relay->setState(0);
          client.println(getState());
        }
        else if (line[1] == '1') {
          relay->setState(1);
          client.println(getState());
        }
        else if (line[1] == '?') {
          client.println(getState());
        }
      }
    }
    client.stop();
  }

  bool getState() {
    return relay->getState();
  }
};

RelayController *controller;

void setup() {
  Serial.begin(9600);
  WiFi.config(IPAddress(10,42,0,200), IPAddress(10,42,0,1), IPAddress(255,255,255,0));
  WiFi.begin("SSID", "PASSWORD");
  WiFi.setAutoReconnect(true);
  if(WiFi.waitForConnectResult() == WL_CONNECTED)
    Serial.println("Connected");
  else
    Serial.println("Failed");
  controller = new RelayController(16, 5, 8000);
}

void loop() {
  controller->stepForward();
  delay(100);
}
