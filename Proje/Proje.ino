#define BLYNK_PRINT Serial
#include "HX711.h"
#include <ESP8266WiFi.h>;
#include <WiFiClient.h>;
#include <ThingSpeak.h>;
#include <Wire.h>
#include <BlynkSimpleEsp8266.h>

#define DOUT  D5
#define CLK  D6

char auth[] = "EBA6IKBdnQS8IcTLrLq2xbMPfKutAXUE";
 
HX711 scale;

WiFiClient client;
const char* ssid = "yasin";
char* password = "08388576bB";

unsigned long channelID = 1256513;
const char * writeAPIKey = "PWP1E3JFJKYBS7F3";

float calibration_factor = 888;

int agirlikDegeri;

String API = "BHWRNS3IVAYSDE03";
const char* thingSpeakHost = "api.thingspeak.com";
String metin;
int twitSayac1 = 0;
int twitSayac2 = 0;
int twitSayac3 = 0;
int twitSayac4 = 0;

void wifiSetup (){

  Serial.println(ssid);
  Serial.println(F("Kablosuz Agina Baglaniyor"));
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print(ssid);
  Serial.println("Kablosuz Aga Baglandi");
  Serial.println("IP adresi: ");
  Serial.println(WiFi.localIP());

}

void setup() {
  Serial.begin(9600);
  scale.begin(DOUT, CLK);
  scale.set_scale();
  scale.tare();
  long zero_factor = scale.read_average();

  wifiSetup();
  ThingSpeak.begin(client);
  Blynk.begin(auth, ssid, password);
}

void loop() {
  Blynk.run();
  agirlik();
 }

void agirlik (){
  scale.set_scale(calibration_factor);
  agirlikDegeri = scale.get_units(), 10;

  if (agirlikDegeri < 0)
  {
    agirlikDegeri = 0;
  }
  if (agirlikDegeri < 100)
  {
    twitSayac2 = 0;
    ThingSpeak.writeField(channelID, 1,agirlikDegeri, writeAPIKey);
    Serial.print("Ağırlık: ");
    Serial.print(agirlikDegeri);
    Serial.print(" gram"); 
    Serial.println();
    if(twitSayac1 == 0){
     metin = "Kap boşaldı. Doldurur musun? #birkapdasendoldur T";
     twitAt(metin,twitSayac3);
     twitSayac3++;
     twitSayac1++;
     Blynk.virtualWrite(V0, agirlikDegeri);
     Blynk.virtualWrite(V1, agirlikDegeri);
     Blynk.notify("Kap boşaldı. Doldurur musun?");

    } 
    delay(100);
   } 
   if (agirlikDegeri >= 100)
   {
     twitSayac1 = 0;
     ThingSpeak.writeField(channelID, 1,agirlikDegeri, writeAPIKey);
     Serial.print("Ağırlık: ");
     Serial.print(agirlikDegeri);
     Serial.print(" gram"); 
     Serial.println();
     if(twitSayac2 == 0){
       metin = "Hayvanlara yardım ettiğin için teşekkürler. #birkapdasendoldur T";
       twitAt(metin,twitSayac4);
       twitSayac4++;
       twitSayac2++;
       Blynk.virtualWrite(V0, agirlikDegeri);
       Blynk.virtualWrite(V1, agirlikDegeri);
       Blynk.notify("Kap doldu! Hayvanlara destek olduğun için teşekkürler");
    }
     delay(100);
   } 
}

void twitAt(String metin, int sayac){
   if(client.connect (thingSpeakHost, 80)){
    String  tsData = "api_key="+API+"&status=" + metin + sayac;

    client.print("POST /apps/thingtweet/1/statuses/update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(tsData.length());
    client.print("\n\n");

    client.print(tsData);
  }
  
}
