#include <U8g2lib.h>

#include <WiFiManager.h>
#include <credentials.h>

//Screen
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define SCREEN_ADDRESS 0x3D ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)

//Icons
#define WIFI_OFF_ICON 57879
#define WIFI_LOW_ICON 57880
#define WIFI_MEDIUM_ICON 57881
#define WIFI_HIGH_ICON 57882
#define ADAFRUIT_CONNECTED_ICON 57643

//Fonts
#define DEFAULT_FONT u8g2_font_t0_16_mf
#define DEFAULT_FONT_SIZE 16

void setupDisplay(U8G2_SSD1306_128X64_NONAME_F_HW_I2C &display, const uint8_t *font){
  
  Serial.println("Setting up display");
  
  display.begin();
  display.clearBuffer();
  display.setFont(font);
  display.sendBuffer();
}

void setWifiIconHigh(U8G2_SSD1306_128X64_NONAME_F_HW_I2C &display){
    display.setFont(u8g2_font_waffle_t_all);
    display.drawGlyph(117,10,WIFI_HIGH_ICON);
    display.sendBuffer();
}

void setWifiIconOff(U8G2_SSD1306_128X64_NONAME_F_HW_I2C &display){
    display.setFont(u8g2_font_waffle_t_all);
    display.drawGlyph(117,10,WIFI_OFF_ICON);
    display.sendBuffer();
}

void setAdafruitConnectedIcon(U8G2_SSD1306_128X64_NONAME_F_HW_I2C &display){
    display.setFont(u8g2_font_waffle_t_all);
    display.drawGlyph(105,10,ADAFRUIT_CONNECTED_ICON);
    display.sendBuffer();
    
}

void printCo2(U8G2_SSD1306_128X64_NONAME_F_HW_I2C &display, uint16_t value){
    display.setCursor(28,36);
    display.setFont(u8g2_font_inr24_mn);
    display.print(value);	// write something to the internal memory
    display.sendBuffer();					// transfer internal memory to the display
}

void printTemp(U8G2_SSD1306_128X64_NONAME_F_HW_I2C &display, uint16_t value){
    display.setCursor(30,65);
    display.setFont(DEFAULT_FONT);
    display.print(value);	// write something to the internal memory
    display.sendBuffer();					// transfer internal memory to the display
}

void printHumidity(U8G2_SSD1306_128X64_NONAME_F_HW_I2C &display, uint16_t value){
    display.setCursor(50,65);
    display.setFont(DEFAULT_FONT);
    display.print(value);	// write something to the internal memory
    display.sendBuffer();					// transfer internal memory to the display
}