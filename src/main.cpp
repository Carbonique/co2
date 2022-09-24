#include <Arduino.h>
#include <WiFiManager.h>
#include <Wire.h>
#include "AdafruitIO_WiFi.h"
#include "SparkFun_SCD30_Arduino_Library.h"
#include "credentials.h"
#include <LiquidCrystal_I2C.h>

AdafruitIO_WiFi io(IO_USERNAME, IO_KEY, "", "");
AdafruitIO_Feed *co2Feed = io.feed("CO2");
AdafruitIO_Feed *temperatureFeed = io.feed("temp_celsius");
AdafruitIO_Feed *relativeHumidityFeed = io.feed("rh_percentage");

SCD30 airSensor;
WiFiManager wifiManager;
LiquidCrystal_I2C lcd(0x61);

bool send = false;

void sendToFeed(AdafruitIO_Feed *feed, int data){
  Serial.printf("sending %d to feed: %s \n", data, feed->name);
  if (send) {
    feed->save(data);
  }

}

void startAirsensor(){

  if (airSensor.begin() == false) {
    Serial.println("Air sensor not detected. Please check wiring. Freezing...");
    while (1)
      ;
  }

}

void setupAdafruitConnection(){

  if (!wifiManager.autoConnect("WiFi Setup")) { // connect to wifi with existing setting or start config
    Serial.println("failed to connect and hit timeout");
  }
  else {
    // if you get here you have connected to the WiFi
    Serial.println("Connected to WiFi.");
    Serial.println("Connecting to Adafruit IO");
    
    io.connect();
    // wait for a connection

    while ((io.status() < AIO_CONNECTED)) {
      Serial.print(".");
      delay(500);
    }
    Serial.println("Connected to Adafruit IO.");
  }

}

void setup() {

  Serial.begin(9600);
  wifiManager.autoConnect("Co2AP", "co2wegermee");
  setupAdafruitConnection();
  Wire.begin();
  startAirsensor();
  
  // Switch on the backlight
  lcd.begin(16,2);               // initialize the lcd 
  lcd.backlight();
} 

void loop() {
  lcd.print("hallo");
  if (airSensor.dataAvailable()) {
    io.run();
  
    sendToFeed(co2Feed, airSensor.getCO2());
//   sendToFeed(temperatureFeed, airSensor.getTemperature());
//   sendToFeed(relativeHumidityFeed, airSensor.getHumidity());

  // Reduce interval to reduce power consumption + to prevent pulling too much current
    delay(5000);
    lcd.clear();
  }
  else {
    Serial.println("Waiting for new data");
    delay(500);
  }
}
