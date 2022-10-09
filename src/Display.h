#include <U8g2lib.h>

//Screen
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define SCREEN_ADDRESS 0x3D ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)

//Positions
#define WIFI_ICON_X 117 
#define WIFI_ICON_Y 10
#define ADAFRUIT_CONNECTED_ICON_X 105
#define ADAFRUIT_CONNECTED_ICON_Y 10

//Icons
#define WIFI_OFF_ICON 57879
#define WIFI_LOW_ICON 57880
#define WIFI_MEDIUM_ICON 57881
#define WIFI_HIGH_ICON 57882
#define ADAFRUIT_CONNECTED_ICON 57643
#define ADAFRUIT_NOT_CONNECTED_ICON 57523

#define TEMP_ICON_FONT u8g2_font_unifont_t_symbols 
#define TEMP_Y 62
#define TEMP_ICON 8451

#define HUMIDITY_ICON_FONT u8g2_font_unifont_t_symbols 
#define HUMIDITY_Y 62
#define HUMIDITY_ICON 37

//Fonts
#define DEFAULT_FONT_MEDIUM u8g2_font_t0_18_mf
#define DEFAULT_FONT_SIZE 18

// https://raw.githubusercontent.com/wiki/olikraus/u8g2/fntpic/u8g2_font_waffle_t_all.png
#define ICON_FONT u8g2_font_waffle_t_all

class Display: public U8G2_SSD1306_128X64_NONAME_F_HW_I2C {
    int current_wifi_icon = 0 ;
    int current_adafruit_connected_icon = 0;
    int last_measured_co2;
    int last_measured_temp;

    bool isCurrentWifiIcon(int new_icon){
        return new_icon == current_wifi_icon;
    }  

    bool isCurrentAdafruitConnectedIcon(int new_icon){
        return new_icon == current_adafruit_connected_icon;
    }

    public:
        using U8G2_SSD1306_128X64_NONAME_F_HW_I2C::U8G2_SSD1306_128X64_NONAME_F_HW_I2C;

        void setup(const uint8_t *font){

          Serial.println("Setting up display");
        
          (*this).begin();
          (*this).clearBuffer();
          (*this).setFont(font);
          (*this).sendBuffer();
        }

        void setWifiIconHigh(){

            if(!isCurrentWifiIcon(WIFI_HIGH_ICON)){
                (*this).setFont(ICON_FONT);
                (*this).drawGlyph(WIFI_ICON_X,WIFI_ICON_Y,WIFI_HIGH_ICON);
                (*this).sendBuffer();
            }
        }

        void setWifiIconMedium(){
            if(!isCurrentWifiIcon(WIFI_MEDIUM_ICON)){
                (*this).setFont(ICON_FONT);
                (*this).drawGlyph(WIFI_ICON_X,WIFI_ICON_Y,WIFI_MEDIUM_ICON);
                (*this).sendBuffer();
            }
        }

        void setWifiIconLow(){
            if(!isCurrentWifiIcon(WIFI_LOW_ICON)){
                (*this).setFont(ICON_FONT);
                (*this).drawGlyph(WIFI_ICON_X,WIFI_ICON_Y,WIFI_LOW_ICON);
                (*this).sendBuffer();
            }
        }

        void setWifiIconOff(){
            if(!isCurrentWifiIcon(WIFI_OFF_ICON)){
                (*this).setFont(ICON_FONT);
                (*this).drawGlyph(WIFI_ICON_X,WIFI_ICON_Y,WIFI_OFF_ICON);
                (*this).sendBuffer();
            }
        }

        void setAdafruitConnectedIcon(){
            if(!isCurrentAdafruitConnectedIcon(ADAFRUIT_CONNECTED_ICON)){
                (*this).setFont(ICON_FONT);
                (*this).drawGlyph(ADAFRUIT_CONNECTED_ICON_X,ADAFRUIT_CONNECTED_ICON_Y,ADAFRUIT_CONNECTED_ICON);
                (*this).sendBuffer();
            }
        }

       void setAdafruitNotConnectedIcon(){
            if(!isCurrentAdafruitConnectedIcon(ADAFRUIT_NOT_CONNECTED_ICON)){
                (*this).setFont(ICON_FONT);
                (*this).drawGlyph(ADAFRUIT_CONNECTED_ICON_X,ADAFRUIT_CONNECTED_ICON_Y,ADAFRUIT_NOT_CONNECTED_ICON);
                (*this).sendBuffer();
            }
        }

        void printCo2(uint16_t value){
            
            //Draw a box to clear screen. Don't do this everytime, as it looks ugly.
            //Clearing the screen is necessary because of number of digits changing
            if(last_measured_co2 >= 1000 && value < 1000){
                //First clear the current text, to make sure 
                //Everything keeps working when cursor moves if ppm < 1000
                (*this).setDrawColor(0);
                (*this).drawBox(12,12,130,30);
                (*this).sendBuffer();
            }

            if(value < 1000){
                (*this).setCursor(36,40);
            } else{
                (*this).setCursor(26,40);
            }

            (*this).setDrawColor(1);
            (*this).setFont(u8g2_font_inr27_mr);
            (*this).print(value);
            (*this).sendBuffer();

            last_measured_co2 = value;
        }

        void printTemp(uint16_t value){

            //Draw a box to clear screen. Don't do this everytime, as it looks ugly.
            //Clearing the screen is necessary because of number of digits changing
            if(last_measured_temp >= 10 && value < 10){
                //First clear the current text, to make sure 
                //Everything keeps working when cursor goes below < 10
                (*this).setDrawColor(0);
                (*this).drawBox(10,TEMP_Y - 20,168,10);
                (*this).sendBuffer();
            }

            if(value < 10){
                (*this).setCursor(30,TEMP_Y);
            } else{
                (*this).setCursor(25,TEMP_Y);
            }


            (*this).setFont(DEFAULT_FONT_MEDIUM);
            (*this).print(value);
            
            (*this).setFont(TEMP_ICON_FONT);
            
            (*this).drawGlyph((*this).getCursorX() + 1, TEMP_Y,TEMP_ICON);
            
            (*this).sendBuffer();
            last_measured_temp = value;
        }

        void printHumidity(uint16_t value){

            (*this).setCursor(80,HUMIDITY_Y);
            (*this).setFont(DEFAULT_FONT_MEDIUM);
            (*this).print(value);

            (*this).setFont(HUMIDITY_ICON_FONT);
            (*this).drawGlyph((*this).getCursorX() + 1,HUMIDITY_Y,HUMIDITY_ICON);

            (*this).sendBuffer();
        }

};