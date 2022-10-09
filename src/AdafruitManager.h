#include <AdafruitIO_WiFi.h>

class AdafruitManager: public AdafruitIO_ESP8266 {
  public:
    using AdafruitIO_ESP8266::AdafruitIO_ESP8266; 
  
    bool isConnected(){
      return AdafruitIO_ESP8266::status() >= AIO_CONNECTED;
    } 

    void sendToFeed(AdafruitIO_Feed *feed, int data){
      Serial.printf("Sending %d to feed: %s \n", data, feed->name);
      feed->save(data);
    }

    void connect(){
    
      Serial.println("Connecting to Adafruit IO");

      AdafruitIO_ESP8266::connect();
      // wait for a connection, but break if it takes too long
      int i = 0;
      bool connected = (*this).isConnected();
      while (!connected) {
        if(i >= 5){
          break;
        }
        Serial.println("Waiting for Adafruit connection");
        i = i + 1;
        delay(500);
      }

      if (connected){
        Serial.println("Connected to Adafruit IO.");
      } else{
        Serial.println("Not connected to Adafruit IO.");
      }
    }
};





