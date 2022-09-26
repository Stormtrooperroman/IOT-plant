
#include <WiFi.h>

#include "config.h"


#include<Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <Adafruit_ADS1015.h>
#include <TLC59108.h>
#include <BH1750FVI.h>

#define pump 17 //пин насоса GP 17
#define RELAY_PIN_1 16
#define RELAY_PIN_2 17


Adafruit_ADS1015 ads(0x48);
Adafruit_BME280 bme280;
BH1750FVI LightSensor_1;

#define HW_RESET_PIN 2
#define I2C_ADDR TLC59108::I2C_ADDR::BASE

TLC59108 leds(I2C_ADDR);

int r, g, b, power;
const float air_value = 1572.9; // 22.9%
const float water_value = 0.0;
const float moisture_0 = 22.9;
const float moisture_100 = 100.0;

bool light = false;


char serverAddress[] = "192.168.2.56/ws/data/";  // server address
int port = 80;

WiFiServer server(80);

int status = WL_IDLE_STATUS;
int count = 0;
String header;
unsigned long currentTime = millis();
unsigned long previousTime = 0;
const long timeoutTime = 2000; 

void setup() {
  Serial.begin(115200);
  bme280.begin();
  ads.setGain(GAIN_TWOTHIRDS);
  ads.begin();
  pinMode(RELAY_PIN_1, OUTPUT);
  pinMode(RELAY_PIN_2, OUTPUT);
  digitalWrite(RELAY_PIN_1, LOW);
  digitalWrite(RELAY_PIN_2, LOW);
  LightSensor_1.begin();
  LightSensor_1.setMode(Continuously_High_Resolution_Mode);

  leds.init(HW_RESET_PIN);
  leds.setLedOutputMode(TLC59108::LED_MODE::PWM_IND);



  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi");
  while(WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");  
  }
  Serial.println("");
  Serial.println("WiFi connected. ");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  server.begin();
}

void loop() {
  float t = bme280.readTemperature();
  float h = bme280.readHumidity();
  float p = bme280.readPressure()  * 0.0075F;
//  float adc0 = (float)ads.readADC_SingleEnded(0) * 6.144 * 16;
//  float adc1 = (float)ads.readADC_SingleEnded(1) * 6.144 * 16;
//  float adc0 = (float)analogRead(SOIL_MOISTURE);
//  float adc1 = (float)ads.readADC_SingleEnded(1) / 2047.0 * 6.144;
//  float adc2 = (float)ads.readADC_SingleEnded(2) / 2047.0 * 6.144;
//  float adc3 = (float)ads.readADC_SingleEnded(3) / 2047.0 * 6.144;
//  float t1 = ((adc1 / 1000));
//  float h1 = map(adc0, air_value, water_value, moisture_0, moisture_100);
  float l = LightSensor_1.getAmbientLight();
  String data = Serial.readString();
  
  if(data == "data"){
    Serial.println("Температура воздуха = " + String(t, 1) + " *C");
    Serial.println("Влажность воздуха = " + String(h, 1) + " %");
    Serial.println("Давление воздуха = " + String(p,  1)  +  " мм рт. ст. ");
//    Serial.println("Влажность почвы = " + String(h1, 1)+ " % ");
//    Serial.println("Температура почвы= " + String(t1, 1)+ " *C");
    Serial.println("Освещённость " + String(l, 1 )+ " Лк");
  }
  else if(data == "pump_on"){
    digitalWrite(RELAY_PIN_1, HIGH); //Включение1  пина
    digitalWrite(RELAY_PIN_2, HIGH); //  Включение2  пина
  }
  else if(data == "pump_off"){
    digitalWrite(RELAY_PIN_1, LOW); //Выключение 1 пина
    digitalWrite(RELAY_PIN_2, LOW); //Выключение 2 пина
  }
  else if(data == "light_on"){
    leds.setAllBrightness(0xfe);
    light = true;
  }
  else if(data == "light_off"){
    byte pwm = 0x00;
    leds.setAllBrightness(pwm);
    light = false;
  }

  WiFiClient client = server.available();

  if (client) {
    currentTime = millis();
    previousTime = currentTime;
    Serial.println("New client.");
    String curentLine = "";
    while (client.connected() && currentTime - previousTime <= timeoutTime){
      currentTime = millis();
      if (client.available()){
        char c = client.read();
        Serial.write(c);
        header += c;
        if ( c == '\n') {
          if (curentLine.length() == 0) {
            if (header.indexOf("GET /data") >= 0) {
              client.println("HTTP/1.1 200 OK");
              client.println("Content-Type: application/json");
              client.println("Connection: close");
              client.println();
              client.println("{\"air_temp\": \"" + String(t, 1)  + "\", \"air_hum\" : \"" + String(h, 1) + "\", \"air_pres\" : \"" + String(p,  1)  + "\", \"light\" : \"" + String(l, 1 ) + "\", \"LED\" : \"" + String(light)+"\" }");
            } else if (header.indexOf("GET /light") >= 0) {
              client.println("HTTP/1.1 200 OK");
              client.println("Connection: close");
              client.println();
              if(light) {
                light = false;
                byte pwm = 0x00;
                leds.setAllBrightness(pwm);
              } else {
                light = true;
                leds.setAllBrightness(0xfe);
              }
              client.println("{\"LED\" : \"" + String(light)+"\" }");
            } else if (header.indexOf("GET /water") >= 0) {
              client.println("HTTP/1.1 200 OK");
              client.println("Content-Type: application/json");
              client.println("Connection: close");
              client.println();
              client.println("{\"water\": \"Add some water\"}");
              digitalWrite(RELAY_PIN_1, HIGH); //Включение1  пина
              digitalWrite(RELAY_PIN_2, HIGH); //  Включение2  пина
              delay(2000);
              digitalWrite(RELAY_PIN_1, LOW); //Выключение 1 пина
              digitalWrite(RELAY_PIN_2, LOW); //Выключение 2 пина

              
            }
            
            break;
          } else {
            curentLine = "";
          }
        } else if (c !=  '\r') {
          curentLine += c;  
        }
      }
    }

    header = "";
    client.stop();
    Serial.println("Client disconnected");
    Serial.println();
  }
}
