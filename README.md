
# About

Project to measure CO2 levels, temperature, and relative humidity using SCD30, SSD1306, and ESP8266.

The project supports multiple modes:
1. Offline mode: no WiFi connection and/or no connection to Adafruit IO.
2. Online mode: WiFi connection and connected to Adafruit IO.

## Offline mode

Data is shown on display only

## Online mode

Data is shown on display and uploaded to Adafruit IO

## Getting started

1. Connect all necessary components. SSD1306 should be connected to D1(SCL) and D2 (SDA). SCD30 should be connected to D6 (SCL) and D7 (SDA)
2. Create the `src/credentials.h` file as specified under [credentials](#credentials)
3. Compile source code and flash to ESP8266
4. Connect to the WiFi AP using details as displayed on the screen (or wait for 180 if you want to run the device in [offline mode](#offline-mode)).
5. Have fun :) 


### Credentials

Create a `src/credentials.h` with the following contents: 
``` c
#define IO_USERNAME  "<YOUR_ADAFRUIT_USERNAME>"
#define IO_KEY       "<YOUR_ADAFRUIT_KEY>"

#define WM_SSID  "<YOUR_WIFIMANAGER_DEFAULT_SSID>"
#define WM_KEY   "<YOUR_WIFIMANAGER_DEFAULT_KEY>"
```