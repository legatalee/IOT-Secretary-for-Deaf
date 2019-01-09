#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>

#define FIREBASE_HOST "luna-ai-secretary.firebaseio.com"
#define FIREBASE_AUTH "sAR8yXUhP0KTmV8FMTfYfnmPwb1KjAvNSbCHNnza"
#define WIFI_SSID "DimiFi_2G"
#define WIFI_PASSWORD "newdimigo"

void setup() {
  Serial.begin(9600);

  pinMode(D1, OUTPUT);
  pinMode(D2, OUTPUT);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("connected: ");
  Serial.println(WiFi.localIP());

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.stream("/queue");
}

int n = 0;

String path;
String data;

void loop() {
  String path = "";
  String data = "";
  if (Firebase.available()) {
    FirebaseObject event = Firebase.readEvent();
    String eventType = event.getString("type");
    eventType.toLowerCase();
    if (eventType == "put") {
      path = event.getString("path");
      data = event.getString("data");
      Serial.print("data: ");
      Serial.println(data);
      Serial.print("data: ");
      Serial.println(path);
    }
  }
  if (data == "light1on") {
    digitalWrite(D1, LOW);
    removeData(path);
  }
  if (data == "light1off") {
    digitalWrite(D1, HIGH);
    removeData(path);
  }
  if (data == "light2on") {
    digitalWrite(D2, LOW);
    removeData(path);
  }
  if (data == "light2off") {
    digitalWrite(D2, HIGH);
    removeData(path);
  }
  if (data == "bye") {
    digitalWrite(D1, HIGH);
    digitalWrite(D2, HIGH);
    removeData(path);
  }
}

void removeData(String path) {
  Firebase.remove("/queue" + path);
}

