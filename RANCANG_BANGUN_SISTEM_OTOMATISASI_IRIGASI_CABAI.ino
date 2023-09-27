#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <Wire.h>
#include <RTClib.h>
#include <DHT.h>

char auth[] = "WZuV2DZeuIuNFjpdEYarRdZ_qTZdLqNX";
char ssid[] = "KD_wifi";  
char pass[] = "120494kd";

BlynkTimer timer;

const int pompapin = 13; //pinD7
const int moisturepin = A0;
int outputvalue = 0;
int persen = 0;

#define DHTPIN D1   
#define DHTTYPE DHT11 
DHT dht(DHTPIN, DHTTYPE);

RTC_DS3231 rtc;
char rt[32];

WidgetLED ledotomatis(V5);
WidgetLED ledmanual(V6);


void sensor1() {
  DateTime now = rtc.now();

  sprintf(rt, "%02d:%02d:%02d",  now.hour(), now.minute(), now.second());  
  
  Serial.print(F("Waktu : "));
  Serial.println(rt);
  
  Blynk.virtualWrite(V4, rt);
  
  if (now.hour() == 7 && now.minute() == 00 && now.second() == 00) {
    while (persen < 70){
    Serial.println ("Waktunya penyiraman");
    Blynk.notify("Waktunya penyiraman");
    
    digitalWrite(pompapin, LOW);
    delay(5000);
    
    outputvalue = analogRead(moisturepin);         
    persen = map(outputvalue, 1023, 0, 45, 100);
    }
  
    digitalWrite(pompapin, HIGH);
  }
}
    

void sensor2 () {
  float t = dht.readTemperature(); 
  float h = dht.readHumidity(); 
  
  if (isnan(h) || isnan(t)) {
    Serial.println(F("DHT11 tidak terbaca!"));
    return;
    }
    
    Blynk.virtualWrite(V1,t);
    Blynk.virtualWrite(V2,h);

    Serial.print("Suhu Udara : ");
    Serial.print(t);
    Serial.println ("°C");
    Serial.println();
    
    Serial.print("Kelembapan Udara : ");
    Serial.print(h);
    Serial.print("%");
    Serial.println();

    if (t > 30) {
      while (persen < 70){
        Serial.println ("Waktunya penyiraman");
        Blynk.notify("Waktunya penyiraman");
        
        digitalWrite(pompapin, LOW);
        delay(5000);
        
        outputvalue = analogRead(moisturepin);         
        persen = map(outputvalue, 1023, 0, 45, 100);
        }
        
        digitalWrite(pompapin, HIGH);
        }
}


void sensor3 () {
  outputvalue = analogRead(moisturepin);         
  persen = map(outputvalue, 1023, 0, 45, 100);
  Serial.print("Kelembapan Tanah  : ");
  Serial.print(persen);
  Serial.println("%");
  
  Blynk.virtualWrite(V0, persen);

  if (persen < 70) {
    Serial.print(persen);
    Serial.println("KERING | POMPA ON");
    Blynk.notify("Siramlah tanamanmu!"); 
    
    digitalWrite(pompapin, LOW);
    ledotomatis.on(); 
    ledmanual.off();
  }
  
  else if (persen > 70 && persen < 78) {
    Serial.print(persen);
    Serial.println("NORMAL");
    
    digitalWrite(pompapin, HIGH);
    ledotomatis.off();
    ledmanual.off();
  }
  
  else if(persen > 80) {
    Serial.print(persen);
    Serial.println("BASAH | POMPA OFF");
    
    digitalWrite(pompapin, HIGH);
    ledotomatis.off();
    ledmanual.off();
  }
}


BLYNK_WRITE(V3) {
  if (param.asInt()==1) {
    ledmanual.on(); 
    ledotomatis.off();
    
    Serial.println("Pompa Manual ON");
    
    digitalWrite(pompapin, LOW);
    delay(3000);
  }
  
  else if (param.asInt()==0) {
    ledmanual.off();
    ledotomatis.off();
    
    Serial.println("Pompa Manual OFF");
    
    digitalWrite(pompapin, HIGH);
    delay(1000);
  }
}


void setup() {
  Serial.begin(9600); 
  Blynk.begin(auth, ssid, pass);  
  
  timer.setInterval(1000L, sensor1); 
  timer.setInterval(1000L, sensor2);
  timer.setInterval(1000L, sensor3);
 
  dht.begin();
  //Serial.println("DHTxx test!");
  
  Wire.begin(4, 0);
  rtc.begin();
  rtc.adjust(DateTime(F(__DATE__),F(__TIME__)));
  //rtc.adjust(DateTime(2021, 8, 4, 6, 50, 50));
  
  pinMode(pompapin,OUTPUT);
  digitalWrite(pompapin, HIGH);
}


void loop()
{
  Blynk.run(); 
  timer.run(); 
}
