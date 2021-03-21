#include <SimpleDHT.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Arduino.h>
#include <PubSubClient.h>
#include <ESP8266WiFi.h>

//define size of OLED-display in pixels:
#define SCREEN_WIDTH 128 
#define SCREEN_HEIGHT 32 
//set OLED_RESET-pin to '-1' since we are sharing the arduino Reset pin:
#define OLED_RESET     -1
//initialize OLED using adafruits display library:
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


//define pin for dht11 (on arduino board)
int pinDHT11 = 2; //gpio 2 ->d4 on esp8266
SimpleDHT11 dht11(pinDHT11);

// network credentials and sqtt-server:
#include "password.h"
const char *password = WIFI_PASSWORD;
const char* ssid = "wlan-antenne";
const char* mqtt_server = "raspi3";
const char* channel = "sensorsESP";
// wohnzimmer, kathi_zimmer
String sensorLocation = "kathi_zimmer";
WiFiClient espHome;
PubSubClient mqttClient(espHome);


/*
 * ### SETUP ###
 */
void setup() {
  Serial.begin(115200);

  // SSD1306_SWITCHCAPVCC = generate OLED-display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  //check Wifi:
  WiFi.begin(ssid, password);
  Serial.print("connect to WLAN");
  while (WiFi.status() != WL_CONNECTED){
    delay(1000);
    Serial.print('_');
  }
  Serial.println("successfully connected to network");
  mqttClient.setServer(mqtt_server, 1883);
  mqttClient.setKeepAlive(0);
  Serial.println("set mqttServer");
  

  // basic settings for OLED-display:
  display.clearDisplay();      // Clear the buffer
  display.setTextSize(2);      // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.setCursor(0, 0);     // Start at top-left corner
  display.cp437(true);         // Use full 256 char 'Code Page 437' font
  display.display();
  
}


/*
 * ### SETUP ###
 */
void loop() {
  // start working...
  Serial.println("=================================");
  Serial.println("connecting");
  if (!mqttClient.connected()){
    reconnect();
    yield();
  }
  Serial.println("read DHT11...");
  
  // get sensor data:
  byte temperature = 0;
  byte humidity = 0;
  int err = SimpleDHTErrSuccess;
  if ((err = dht11.read(&temperature, &humidity, NULL)) != SimpleDHTErrSuccess) {
    Serial.print("Read DHT11 failed, err="); Serial.println(err);delay(1000);
    return;
  }

  //print to oled:
  //display.setCursor(32, 0);
  display.clearDisplay();
  
  display.setCursor(0,0);
  display.print("TEM: ");
  display.println((int) temperature);
  display.setCursor(0,14);
  display.print("HUM: ");
  display.println((int) humidity);
  display.display();
  //print to serial:
  Serial.print("Sample OK: ");
  Serial.print((int)temperature); Serial.print(" *C, "); 
  Serial.print((int)humidity); Serial.println(" H");
  //send to server:
  String tempText = ("temp,location=" + sensorLocation + " value=" + String(temperature));
  mqttClient.publish(channel, tempText.c_str(), true);
  String humText = ("hum,location=" + sensorLocation + " value=" + String(humidity));
  mqttClient.publish(channel, humText.c_str(), true);
  
  // wait 5 mins:
  delay(300000);
}

/*
 * ### reconnecting to sqtt-server ###
 */

 void reconnect(){
  while (!mqttClient.connected()){
    Serial.println("Trying to connect to MQTT-server");
  //create new and random client-ID for connection:
  String clientId = "ESP8266-" + String(random(0xffff), HEX);
  //connect:
  if (!mqttClient.connect(clientId.c_str())){
    Serial.print("-");
    delay(5000);
  }
  yield();
  Serial.println("connected");
 }}
