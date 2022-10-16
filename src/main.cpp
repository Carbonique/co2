#include <Arduino.h>
#include <Wire.h>
#include <AdafruitManager.h>
#include <Display.h>
#include <WiFiManager.h>
#include <SparkFun_SCD30_Arduino_Library.h>
#include <credentials.h>
#include "AdafruitIO.h"

#define SDA_1 4 //D2, default pin for SDA
#define SCL_1 5 //D1, default pin for SCL

#define SDA_2 13 //D7
#define SCL_2 12 //D6

TwoWire Wire1 = TwoWire();
SCD30 airSensor;
Display display(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);
AdafruitManager adafruitManager(IO_USERNAME, IO_KEY,"","");

AdafruitIO_Feed *co2Feed = adafruitManager.feed("CO2");
AdafruitIO_Feed *temperatureFeed = adafruitManager.feed("temp_celsius");
AdafruitIO_Feed *relativeHumidityFeed = adafruitManager.feed("rh_percentage");

// Function declaration
void setupAirsensor(SCD30 &airSensor);
void configModeCallback (WiFiManager *wm);
void setupWiFiManager(WiFiManager &wm, Display &display);
bool wifiIsConnected();
int getQuality();
void setWifiIconBasedOnQuality(Display &display);
void setAdafruitIconBasedOnConnection(Display &display, bool connected);

void setup() {
  
  Serial.begin(9600);
  //Would be better if we could pass the Wire object to setup(), but I never got that working
  Wire.begin(SDA_1, SCL_1);
  display.setup(u8g2_font_inr33_mn);
  
  //WiFiManager, Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;
  setupWiFiManager(wifiManager, display);
  
  if(wifiIsConnected()){
    adafruitManager.connect();
  }

  //Would be better if we could pass the Wire object to setupAirsensor(), but I never got that working
  Wire.begin(SDA_2, SCL_2);
  setupAirsensor(airSensor);

  //Clear display to make sure the loop() start from a cleared screen 
  display.clearDisplay(); 
} 

void loop() {

  Wire.begin(SDA_1, SCL_1);

  // First check wifi connection + print wifi icon
  bool wifiConnected = wifiIsConnected();
  setWifiIconBasedOnQuality(display);

  // Now check whether Adafruit connection is running

  bool adafruitConnected; 

  switch (wifiConnected) {
    case false:
      adafruitConnected = false;
      break;
    case true:
      // Fail fast to prevent long waiting if not connected.
      adafruitManager.run(0U, true);
      adafruitConnected = adafruitManager.isConnected();
      break;
  }

  setAdafruitIconBasedOnConnection(display, adafruitConnected);

  Wire.begin(SDA_2, SCL_2);
  // Now that we know all connnection statuses, it's time to do some measurements
  uint16_t co2;
  uint16_t temp;
  uint16_t humidity;
  
  // We use proceed as to resemble an 'exit if'. 
  // If no data is available, we will 'exit' the loop
  bool proceed = false;

  if (airSensor.dataAvailable()) {
    co2 = airSensor.getCO2();
    temp = airSensor.getTemperature();
    humidity = airSensor.getHumidity();
    proceed = true;
  }
  
  Wire.begin(SDA_1, SCL_1);

  switch (proceed) {
    case false:
      Serial.println("Waiting for new data");
      delay(500);
      break;

    case true:

      if(adafruitConnected){
        adafruitManager.sendToFeed(co2Feed, co2);
        adafruitManager.sendToFeed(temperatureFeed, temp);
        adafruitManager.sendToFeed(relativeHumidityFeed, humidity);
      }
      display.printCo2(co2);
      display.printTemp(temp);
      display.printHumidity(humidity);
      delay(10000);

      break;
  } 
}

void setupAirsensor(SCD30 &airSensor){
  Serial.println("Starting airsensor");

  if (airSensor.begin() == false) {
    Serial.println("Air sensor not detected. Please check wiring. Freezing...");
    while (1)
      ;
  }
}

void configModeCallback (WiFiManager *wm) {
  Serial.println("Entered config mode");
  //It's not possible to pass display as param, so we use display 'implicitly' 
  
  display.clearBuffer();					// clear the internal memory
  display.setFont(u8g2_font_t0_14_mf);
  display.setCursor(0, 15);
  display.printf("SSID: %s", wm->getConfigPortalSSID().c_str());
  //14 is font size
  display.setCursor(0, (-1 * display.getDescent()) + 14 + display.getCursorY());
  display.printf("Key: %s", WM_KEY);
  display.setCursor(0, (-1 * display.getDescent()) + 14 + display.getCursorY());
  display.printf("IP: %s", WiFi.softAPIP().toString().c_str());
  display.sendBuffer();					// transfer internal memory to the display
}

void setupWiFiManager(WiFiManager &wm, Display &display){

  Serial.println("Starting WifiManager");

  WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP    

  wm.setAPCallback(configModeCallback);
  wm.setConfigPortalTimeout(180);
  
  bool res;
  
  res = wm.autoConnect(WM_SSID, WM_KEY);
  
  if(!res) {
    Serial.println("Failed to connect");
    display.clearBuffer();
    display.setFont(u8g2_font_t0_14_mf);
    display.setCursor(0, 10);
    display.print("WiFi connection");
    display.setCursor(0, (-1 * display.getDescent()) + 14 + display.getCursorY());
    display.print("failed.");
    display.sendBuffer();
    delay(5000);
  } 
    else {
    Serial.println("WiFi connected");
  }
}

bool wifiIsConnected(){
  return WiFi.status() == WL_CONNECTED;
}

// Source: https://github.com/tttapa/Projects/blob/master/ESP8266/WiFi/RSSI-WiFi-Quality/RSSI-WiFi-Quality.ino
/*
   Return the quality (Received Signal Strength Indicator)
   of the WiFi network.
   Returns a number between 0 and 100 if WiFi is connected.
   Returns -1 if WiFi is disconnected.
*/
int getQuality() {
  if (WiFi.status() != WL_CONNECTED)
    return -1;
  int dBm = WiFi.RSSI();
  if (dBm <= -100)
    return 0;
  if (dBm >= -50)
    return 100;
  return 2 * (dBm + 100);
}

void setWifiIconBasedOnQuality(Display &display){
  int quality = getQuality();

  switch(quality){
    case -1:
      Serial.println("No wifi connected. Setting icon to off.");
      display.setWifiIconOff();
      break;
    case 0:
      Serial.printf("Wifi is connected, signal is %d \n", quality);
      display.setWifiIconOff();
      break;
    case 70 ... 100:
      Serial.printf("Wifi is connected, signal is %d \n", quality);
      display.setWifiIconHigh();
      break;
    case 40 ... 69:
      Serial.printf("Wifi is connected, signal is %d \n", quality);
      display.setWifiIconMedium();
      break;
    case 1 ... 39:
      Serial.printf("Wifi is connected, signal is %d \n", quality);
      display.setWifiIconLow();
      break;
  }

}

void setAdafruitIconBasedOnConnection(Display &display, bool connected){
  if(connected){
    display.setAdafruitConnectedIcon();
  } else{
    display.setAdafruitNotConnectedIcon();
  }
}