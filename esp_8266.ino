#include <Wire.h>
#include "FirebaseESP8266.h"
#include "ESP8266WiFi.h"
#include "ESP8266HTTPClient.h"

#define FIREBASE_HOST "************************************"
#define FIREBASE_AUTH "************************************"
#define HOST "api.thingspeak.com"
#define WIFI_SSID "INFINIA-Hackathon-1"
#define WIFI_PASSWORD "infinia#2023"

unsigned long lastTime = 0;
int resetTime;

FirebaseData tempData;
FirebaseData DataChange;
FirebaseData updateData;

String tempReaded;

HTTPClient http;
WiFiClient client;

String clearData(String data){
  String out = data;
  out.replace("\\\"","");
  return out;
}

void setup(){
  Serial.begin(9600);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Aga baglaniliyor ");

  while(WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(200);
  }
  Serial.println("/n/r Baglandi!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);

  Firebase.setString(DataChange, "/lcd-string-change", "1");

  if(Firebase.getString(updateData, "/lcd-update-time")){
    resetTime = clearData(updateData.stringData()).toInt();
    Serial.println("ResetTime data collecting");
    Serial.println(resetTime);
  }
}

void loop(){

  tempReaded = Serial.read();

  unsigned long currentMillis = millis();
  if(currentMillis >= resetTime * 1000 + lastTime){
    lastTime = currentMillis;
    Serial.println("data check starting");
    Firebase.setString(DataChange, "/lcd-string-change", "1");
  }

  while(!client.connect(HOST, 80)){
    client.connect(HOST, 80);
    delay(150);
  }

  if(Firebase.getString(DataChange, "/lcd-string-change")){
    while(DataChange.stringData() == "1"){
      if(Firebase.getString(tempData, "/lcd-temp")){


        // Data will come with \"example\" style, so data have to be cleared.
        String fixData = clearData(tempReaded);

        Serial.println(fixData);
        Firebase.setString(tempData, "/lcd-temp", fixData);
        Firebase.setString(DataChange, "/lcd-string-change", "0");
      }

      if(Firebase.getString(updateData, "/lcd-update-time")){
        resetTime = clearData(updateData.stringData()).toInt();
        Serial.println("Data refreshing");
        Serial.println(resetTime);
      }

      else{
        Serial.println(tempData.errorReason());
        Firebase.getString(DataChange, "/lcd-string-change");
      }
    }
  }
}