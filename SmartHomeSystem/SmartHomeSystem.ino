#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <FS.h>
#include <DHT.h>
#include "DHT.h"
#define DhtPin D3
#define DhtType DHT11
#define LightSensorPin A0
#define Bulb D4
#define led1 D5
int light;
int cm;
int pot;
int led;
int trigPin=16;
int echoPin=5;
int Buzzer=12;
const char* ssid = "CANALBOX-BA81";
const char* password = "8955819532";

DHT dht(DhtPin, DhtType);
AsyncWebServer server(80);

//SSID and Password of your WiFi router


unsigned long previousMillis = 0;    // will store last time DHT was updated

// Updates DHT readings every 10 seconds
const long interval = 2000; 
int b;
int duration;
float t=0.0;
float h=0.0;
void setup() {
  Serial.begin(115200);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(LightSensorPin,INPUT);
  pinMode(A0,INPUT);
  pinMode(2,OUTPUT);
  pinMode(Bulb, OUTPUT);
  pinMode(Buzzer, OUTPUT);
  WiFi.begin(ssid, password);     //Connect to your WiFi router
  Serial.println("");
   while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  //If connection successful show IP address in serial monitor
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());  //IP address assigned to your ESP
  SPIFFS.begin();

  // Route to load index.html file and all file 
  server.on("*", HTTP_GET, [](AsyncWebServerRequest *request){
    if(request->url() == "/data") {
       AsyncWebServerResponse *response = request->beginResponse(200, "Application/json","{\"temp\":"+String(t)+",\"bulb\":"+String(b)+",\"humidity\":"+String(h)+",\"distance\":"+cm+",\"light\":"+pot+"}");
       response->addHeader("Access-Control-Allow-Origin","*");
       request->send(response);
    }
    else if(request->url() == "/"){
      request->send(SPIFFS, "/index.html", String()); // in some casees when you change directory to 192.168.4.1/ it will not work so this line is to fix that problem
    }
    else if(request->url() == "/bulb"){
      if(digitalRead(Bulb) == LOW){
        digitalWrite(Bulb,HIGH);
        AsyncWebServerResponse *response = request->beginResponse(200, "Application/json","{\"state\":1}");
        response->addHeader("Access-Control-Allow-Origin","*");
        request->send(response);
      }
      else{
        digitalWrite(Bulb,LOW);
        AsyncWebServerResponse *response = request->beginResponse(200, "Application/json","{\"state\":0}");
        response->addHeader("Access-Control-Allow-Origin","*");
        request->send(response);
      }
    }
   else if(request->url() == "/buzzer"){
      if(digitalRead(Buzzer) == LOW){
        digitalWrite(Buzzer,HIGH);
        AsyncWebServerResponse *response = request->beginResponse(200, "Application/json","{\"state\":0}");
        response->addHeader("Access-Control-Allow-Origin","*");
        request->send(response);
      }
      else{
        digitalWrite(Buzzer,LOW);
        AsyncWebServerResponse *response = request->beginResponse(200, "Application/json","{\"state\":1}");
        response->addHeader("Access-Control-Allow-Origin","*");
        request->send(response);
      }
    }
    else{
       request->send(SPIFFS, request->url(), String());
    }
  });
  server.begin();
  dht.begin();

}
 
void loop() {
  
digitalWrite(trigPin, LOW);
delayMicroseconds(5);
digitalWrite(trigPin, HIGH);
delayMicroseconds(10);
digitalWrite(trigPin, LOW);
pinMode(echoPin,INPUT);
duration=pulseIn(echoPin, HIGH);
  cm =(duration/2)*0.0343; // Divide by 29.1 or multiply by 0.0343
  if(cm>14){
  digitalWrite(Buzzer, HIGH);
  }
  else{
    digitalWrite(Buzzer, LOW);
    }
    delay(2000);
  pot=analogRead(A0);
  led=map(pot ,0 , 1023 ,0 , 255);
  analogWrite(led1,led);
  b=digitalRead(Bulb);
  // Read temperature as Celsius (the default)
  t = dht.readTemperature();
  // Read Humidity
  h = dht.readHumidity();
  Serial.println("Temperature =" + String(t));
  Serial.println("Humidity =" + String(h));
  Serial.println("Distance =" + String(cm));
  Serial.println("pot =" + String(led));
  delay(2000);
}
