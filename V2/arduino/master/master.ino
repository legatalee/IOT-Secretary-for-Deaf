#include <WiFiClient.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <FirebaseArduino.h>
#include <SoftwareSerial.h>

#include "DHT.h"

#define FIREBASE_HOST "luna-ai-secretary.firebaseio.com"
#define FIREBASE_AUTH "sAR8yXUhP0KTmV8FMTfYfnmPwb1KjAvNSbCHNnza"
#define WIFI_SSID "DimiFi_2G"
#define WIFI_PASSWORD "newdimigo"

SoftwareSerial HC12(0, 2);

DHT dht(5, DHT11);

int measurePin = A0;
int ledPower = 16;

int samplingTime = 280;
int deltaTime = 40;
int sleepTime = 9680;

HTTPClient http;

void setup() {
  Serial.begin(115200);
  HC12.begin(9600);

  pinMode(ledPower, OUTPUT);

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

  dht.begin();
}


String path;
String data;

void loop() {
//  float humi = dht.readHumidity();
//  float temp = dht.readTemperature();
//  Serial.println(readDust());
//  Serial.println(humi);
//  Serial.println(temp);
//  Serial.println();
//  delay(100);

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
    Serial.println("on!");
    HC12.write("1on");
    removeData(path);
  }
  if (data == "light1off") {
    HC12.write("1off");
    removeData(path);
  }
  if (data == "light2on") {
    HC12.write("2on");
    removeData(path);
  }
  if (data == "light2off") {
    HC12.write("2off");
    removeData(path);
  }
  if (data == "electon") {
    HC12.write("von");
    removeData(path);
  }
  if (data == "electoff") {
    HC12.write("voff");
    removeData(path);
  }
  if (data == "bye") {
    HC12.write("1off");
    delay(10);
    HC12.write("2off");
    delay(10);
    HC12.write("voff");

    removeData(path);
  }
  if (data == "dust") {
    int ugm3 = readDust();
    String dust = String(ugm3);
    String msg = "현재 실내 미세먼지 수치는 ";
    msg += dust;
    msg += "으로 ";
    if (ugm3 < 50) {
      msg += "쾌적한 편입니다.";
    }
    else if ((ugm3 >= 50) && (ugm3 < 100)) {
      msg += "양호한 편입니다.";
    }
    else if (ugm3 >= 100) {
      msg += "매우 위험한 수준입니다. 지금 당장 환기하십시요.";
    }
    String postData = "msg=" + msg;

    http.begin("https://us-central1-luna-ai-secretary.cloudfunctions.net/sendResponseMsg/dust");
    http.addHeader("Content-type", "application/x-www-form-urlencoded");

    int httpCode = http.POST(postData);
    String payload = http.getString();
    Serial.println(httpCode);
    Serial.println(payload);
    http.end();
    removeData(path);
  }

  if (data == "environment") {
    float humi = dht.readHumidity();
    float temp = dht.readTemperature();
    String msg = "현재 실내 온도는 ";
    msg += temp;
    msg += "도이고, 실내 습도는 ";
    msg += humi;
    msg += "퍼센트입니다.";
    String postData = "msg=" + msg;

    http.begin("https://us-central1-luna-ai-secretary.cloudfunctions.net/sendResponseMsg/environment");
    http.addHeader("Content-type", "application/x-www-form-urlencoded");

    int httpCode = http.POST(postData);
    String payload = http.getString();
    Serial.println(httpCode);
    Serial.println(payload);
    http.end();
    removeData(path);
  }

  if (data == "search1") {
    HC12.write("search");
    removeData(path);
  }
}

void removeData(String path) {
  Firebase.remove("/queue" + path);
}

int readDust() {
  float voMeasured = 0;
  float calcVoltage = 0;
  float dustDensity = 0;

  digitalWrite(ledPower, LOW);
  delayMicroseconds(samplingTime);
  voMeasured = analogRead(measurePin);
  delayMicroseconds(deltaTime);
  digitalWrite(ledPower, HIGH);
  delayMicroseconds(sleepTime);
  calcVoltage = voMeasured * (5.0 / 1024.0);
  dustDensity = 100 * calcVoltage;

  return dustDensity;
}
