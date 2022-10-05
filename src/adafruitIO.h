#include <AdafruitIO_WiFi.h>
#include <credentials.h>

AdafruitIO_WiFi adafruitIO(IO_USERNAME, IO_KEY, "", "");
AdafruitIO_Feed *co2Feed = adafruitIO.feed("CO2");
AdafruitIO_Feed *temperatureFeed = adafruitIO.feed("temp_celsius");
AdafruitIO_Feed *relativeHumidityFeed = adafruitIO.feed("rh_percentage");

void sendToFeed(AdafruitIO_Feed *feed, int data){
  Serial.printf("Sending %d to feed: %s \n", data, feed->name);
  feed->save(data);
}

bool AdafruitIOConnected(AdafruitIO &io){
  return io.status() >= AIO_CONNECTED;
}

void setupAdafruitConnection(AdafruitIO &io){

  Serial.println("Connecting to Adafruit IO");
    
  io.connect();
  // wait for a connection, but break if it takes too long
  int i = 0;
  bool connected = AdafruitIOConnected(io);
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

