#include "SparkFun_SCD30_Arduino_Library.h" 

void setupAirsensor(SCD30 &airSensor){
  Serial.println("Starting airsensor");

  if (airSensor.begin() == false) {
    Serial.println("Air sensor not detected. Please check wiring. Freezing...");
    while (1)
      ;
  }
}
