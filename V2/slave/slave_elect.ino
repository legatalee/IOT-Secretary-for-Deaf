#include <SoftwareSerial.h>

SoftwareSerial HC12(3, 2);

void setup() {
  Serial.begin(9600);
  HC12.begin(9600);

  pinMode(5, OUTPUT);
}

void loop() {
  String input = "";
  while (HC12.available()) {
    input += (char) HC12.read();
    delay(1);
  }
  if (input == "von") {
    digitalWrite(5, HIGH);
  }
  if (input == "voff") {
    digitalWrite(5, LOW);
  }
}

