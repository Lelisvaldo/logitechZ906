#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "fauxmoESP.h"

#define ssid "PSD-HOME"
#define pass "PSD@85416346"
#define logiZ906 "Logitech-Z906"

const int buttonPin = 5;                // the number of the PUSHBUTTON pin
const int relayPin =  4;                // the number of the RELAY pin

// variables will change:
int buttonState = 0;                    // variable for reading the pushbutton status
int alexaState = 0;                    // variable for reading the alexaState status

bool button;
bool alexa;

fauxmoESP fauxmo;

void wifi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
  }
}

void setup() {
  wifi();
  Serial.begin(115200);
  pinMode(relayPin, OUTPUT);            // initialize the relayPin pin as an output:
  pinMode(buttonPin, INPUT);            // initialize the pushbutton pin as an input:
  fauxmo.createServer(true);
  fauxmo.setPort(80);
  fauxmo.enable(true);
  fauxmo.addDevice(logiZ906);

  fauxmo.onSetState([](unsigned char device_id, const char * device_name, bool state, unsigned char value) {
    Serial.printf("[MAIN](%s) state: %s\n", device_name, state ? "ON" : "OFF");
    if (strcmp(device_name, logiZ906) == 0) {
      if (state) {
        alexa = true;
      } else {
        alexa = false;
      }
    }
  });
}

void loop() {
  fauxmo.handle();
  button = digitalRead(buttonPin);
  if (buttonState != button) {
    commandChange();
  }
  else if (alexaState != alexa) {
    commandChange();
    alexaState = !alexaState;
  }
}

void commandChange() {
  digitalWrite(relayPin, HIGH);
  fauxmo.setState(logiZ906, true, 255);
  delay(500);
  digitalWrite(relayPin, LOW);
  fauxmo.setState(logiZ906, false, 0);
}
