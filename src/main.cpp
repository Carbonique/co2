#include <Arduino.h>
#include <Wire.h>
#include <adafruitIO.h>
#include <display.h>
#include <scd30.h>
#include <wifi.h>

#define SDA_1 4 //D2, default pin for SDA
#define SCL_1 5 //D1, default pin for SCL

#define SDA_2 13 //D7
#define SCL_2 12 //D6

SCD30 airSensor;
U8G2_SSD1306_128X64_NONAME_F_HW_I2C display(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

bool send = false;
bool WifiConnected;

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

void setup() {
  
  Serial.begin(9600);
  Wire.begin();
  
  //Would be better if we could pass the Wire object to setupDisplay(), but I never got that working
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
  Serial.println("Loop()");

  uint16_t co2;
  uint16_t temp;
  uint16_t humidity;
  // We use proceed as for an 'exit' if. If no data is available, we will 'exit' the loop
  bool proceed = false;
  if (airSensor.dataAvailable()) {
    co2 = airSensor.getCO2();
    temp = airSensor.getTemperature();
    humidity = airSensor.getHumidity();
    proceed = true;
  }

  if(proceed){

    if(WifiConnected){
      setWifiIconHigh(display);

      adafruitIO.run();
      
      if(AdafruitIOConnected(adafruitIO)){
        setAdafruitConnectedIcon(display);
        //sendToFeed(co2Feed, co2);
      }
    } else {
      setWifiIconOff(display);
    }
    printCo2(display, co2);
    printTemp(display, temp);
    printTemp(display, humidity);

    delay(5000);
  } else {
    Serial.println("Waiting for new data");
    delay(500);
  }
}
