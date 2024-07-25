#include <WiFi.h>
WiFiClient client;
const char* MY_SSID = //"Wifi name"
const char* MY_PWD = //"Wifi password"
const char* TS_SERVER = "api.thingspeak.com";
String TS_API_KEY =//"ThingSpeak API key";
#include "DHT.h"
#define DHTPIN 2// DHT11 Pin
#define DHTTYPE DHT11
#define POWER_PIN  17 // VCC pin
#define SIGNAL_PIN 34//WaterLevel Pin
DHT dht(DHTPIN, DHTTYPE);
int value = 0;
const int buzzerPin = 13;//buzzer
const int l1=5;//led 1
const int l2=4;//led 2
const int l3=18;//led 3
void connectWifi()
{
  Serial.print("Connecting to "+ *MY_SSID);
  WiFi.begin(MY_SSID, MY_PWD);
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi Connected");
  Serial.println("");  
}
void setup() {
  Serial.begin(9600);
  Serial.println(F("DHTxx test!"));
  Serial.begin(9600);
  pinMode(POWER_PIN, OUTPUT);   // configure pin as an OUTPUT
  pinMode(buzzerPin, OUTPUT);
  pinMode(l1,OUTPUT);
  pinMode(l2,OUTPUT);
  pinMode(l3,OUTPUT);
  digitalWrite(POWER_PIN, LOW);
  dht.begin();
  delay(700);
  connectWifi();
}
void loop() 
{
  delay(2000);
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  float f = dht.readTemperature(true);
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }
  float hif = dht.computeHeatIndex(f, h);
  float hic = dht.computeHeatIndex(t, h, false);
  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.print(F("°C "));
  Serial.print(f);
  Serial.print(F("°F  Heat index: "));
  Serial.print(hic);
  Serial.print(F("°C "));
  Serial.print(hif);
  Serial.println(F("°F"));
  digitalWrite(POWER_PIN, HIGH);  // turn the sensor ON
  delay(10);                      // wait 10 milliseconds
  value = analogRead(SIGNAL_PIN); // read the analog value from sensor
  digitalWrite(POWER_PIN, LOW);   // turn the sensor OFF

  Serial.print("The water sensor value: ");
  Serial.println(value);
  if(h<30)//min humidity value
  {
  digitalWrite(l1,HIGH);
  digitalWrite(l2,LOW);
  digitalWrite(l3,LOW);
  digitalWrite(buzzerPin, HIGH);
  delay(100);
  }
  else if(h>=30&&h<=60)//correct Humidity
  {
  digitalWrite(l1,HIGH);
  digitalWrite(l2,HIGH);
  digitalWrite(l3,LOW);
  digitalWrite(buzzerPin, LOW);
  delay(100);
  }
  else//maximum humidity
  {
  digitalWrite(l1,HIGH);
  digitalWrite(l2,HIGH);
  digitalWrite(l3,HIGH);
  digitalWrite(buzzerPin,HIGH);
  delay(100);  
  }
  delay(1000);
  if (client.connect(TS_SERVER, 80)) 
  { 
     String postStr = TS_API_KEY;
     postStr += "&field1=";
     postStr += String(value);
     postStr += "&field2=";
     postStr += String(t);
     postStr += "&field3=";
     postStr += String(h);
     postStr += "\r\n\r\n";
   
     client.print("POST /update HTTP/1.1\n");
     client.print("Host: api.thingspeak.com\n");
     client.print("Connection: close\n");
     client.print("X-THINGSPEAKAPIKEY: " + TS_API_KEY + "\n");
     client.print("Content-Type: application/x-www-form-urlencoded\n");
     client.print("Content-Length: ");
     client.print(postStr.length());
     client.print("\n\n");
     client.print(postStr);
     delay(1000); 
   }
   client.stop();
}