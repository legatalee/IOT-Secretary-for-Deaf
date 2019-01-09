#include <SoftwareSerial.h>

SoftwareSerial HC12(2, 3);

void setup() {
  Serial.begin(9600);
  HC12.begin(9600);

  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  String input = "";
  while (HC12.available()) {
    input += (char) HC12.read();
    Serial.println(input);
    delay(1);
  }
  if (input == "1on") {
    digitalWrite(5, HIGH);
    digitalWrite(LED_BUILTIN, HIGH);
  }
  if (input == "1off") {
    digitalWrite(5, LOW);
    digitalWrite(LED_BUILTIN, LOW);
  }
  if (input == "2on") {
    digitalWrite(6, HIGH);
  }
  if (input == "2off") {
    digitalWrite(6, LOW);
  }
}

