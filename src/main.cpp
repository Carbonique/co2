#include <Arduino.h>
#include <WiFiManager.h>
#include <Wire.h>
#include <AdafruitIO_WiFi.h>
#include <credentials.h>
#include "SparkFun_SCD30_Arduino_Library.h" 
#include <U8g2lib.h>

#define SDA_1 4 //D2, default pin for SDA
#define SCL_1 5 //D1, default pin for SCL

#define SDA_2 13 //D7
#define SCL_2 12 //D6

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define SCREEN_ADDRESS 0x3D ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)

#define DEFAULT_FONT u8g2_font_t0_16_mf
#define DEFAULT_FONT_SIZE 16

#define WIFI_OFF_ICON 57879
#define WIFI_LOW_ICON 57880
#define WIFI_MEDIUM_ICON 57881
#define WIFI_HIGH_ICON 57882
#define ADAFRUIT_CONNECTED_ICON 57643

AdafruitIO_WiFi adafruitIO(IO_USERNAME, IO_KEY, "", "");
AdafruitIO_Feed *co2Feed = adafruitIO.feed("CO2");
AdafruitIO_Feed *temperatureFeed = adafruitIO.feed("temp_celsius");
AdafruitIO_Feed *relativeHumidityFeed = adafruitIO.feed("rh_percentage");

SCD30 airSensor;
U8G2_SSD1306_128X64_NONAME_F_HW_I2C display(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

bool send = false;
bool WifiConnected;

void sendToFeed(AdafruitIO_Feed *feed, int data){
  Serial.printf("Sending %d to feed: %s \n", data, feed->name);
  if (send) {
    feed->save(data);
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
  display.setFont(DEFAULT_FONT);
  display.setCursor(0, 15);
  display.printf("SSID: %s", WM_SSID);
  display.setCursor(0, (-1 * display.getDescent()) + DEFAULT_FONT_SIZE + display.getCursorY());
  display.printf("Key: %s", WM_KEY);
  display.setCursor(0, (-1 * display.getDescent()) + DEFAULT_FONT_SIZE + display.getCursorY());
  display.printf("IP: %s", "192.168.4.1");
  display.sendBuffer();					// transfer internal memory to the display
}

bool setupWiFiManager(WiFiManager &wm, U8G2_SSD1306_128X64_NONAME_F_HW_I2C &display){

  Serial.println("Starting WifiManager");

  WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP    

  wm.setAPCallback(configModeCallback);
  wm.setConfigPortalTimeout(180);
  
  bool res;
      
  res = wm.autoConnect(WM_SSID, WM_KEY);
  
  if(!res) {
    Serial.println("Failed to connect");
    display.clearBuffer();					// clear the internal memory
    display.setFont(DEFAULT_FONT);
    display.setCursor(0, 10);
    display.print("WiFi connection");
    display.setCursor(0, (-1 * display.getDescent()) + DEFAULT_FONT_SIZE + display.getCursorY());
    display.print("failed.");
    display.sendBuffer();
    delay(5000);
  } 
    else {
    Serial.println("WiFi connected");
  }
  return res;
}

void setupAdafruitConnection(AdafruitIO_WiFi &io){

  Serial.println("Connecting to Adafruit IO");
    
  io.connect();
  // wait for a connection

  while ((io.status() < AIO_CONNECTED)) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("Connected to Adafruit IO.");
  
}

void setupDisplay(U8G2_SSD1306_128X64_NONAME_F_HW_I2C &display, const uint8_t *font){
  
  Serial.println("Setting up display");
  
  display.begin();
  display.clearBuffer();
  display.setFont(font);
  display.sendBuffer();
}

void setup() {
  
  Serial.begin(9600);

  Wire.begin();
  
  //Would be better if we could pass the Wire object to setupDisplay(), but I never got that working  setupDisplay(display);
  setupDisplay(display, u8g2_font_inr33_mn);
  
  //WiFiManager, Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;

  WifiConnected = setupWiFiManager(wifiManager, display);
  
  if(WifiConnected){
    setupAdafruitConnection(adafruitIO);
  }

  //Would be better if we could pass the Wire object to setupAirsensor(), but I never got that working
  setupAirsensor(airSensor);
  
} 

void loop() {

    if(WifiConnected){
      display.clearBuffer();					// clear the internal memory
      display.setFont(u8g2_font_waffle_t_all);
      display.drawGlyph(117,10,WIFI_HIGH_ICON);
      display.drawGlyph(105,10,ADAFRUIT_CONNECTED_ICON);
      display.sendBuffer();
    } else {
      display.clearBuffer();					// clear the internal memory
      display.setFont(u8g2_font_waffle_t_all);
      display.drawGlyph(117,10,WIFI_OFF_ICON);
      display.sendBuffer();
    }

  if (airSensor.dataAvailable()) {

    uint16_t co2 = airSensor.getCO2();
    display.setCursor(10,48);
    display.setFont(u8g2_font_inr33_mn);
    display.print(co2);	// write something to the internal memory
    display.sendBuffer();					// transfer internal memory to the display

    if(WifiConnected){
      adafruitIO.run();
      sendToFeed(co2Feed, co2);
    }
  

//   sendToFeed(temperatureFeed, airSensor.getTemperature());
//   sendToFeed(relativeHumidityFeed, airSensor.getHumidity());//
  // Reduce interval to reduce power consumption + to prevent pulling too much current
    delay(5000);
  }
  else {
    Serial.println("Waiting for new data");
    delay(500);
  }
}
