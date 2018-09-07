#include <OneWire.h>
#include <DallasTemperature.h>

int tempPin = 2;

OneWire oneWire(2);

DallasTemperature sensors(&oneWire);

void setup() {
  Serial.begin(9600);
  //initialize the temp sensor unit
  sensors.begin();
}

void loop() {
    sensors.requestTemperatures(); 

    Serial.println(sensors.getTempFByIndex(0));
    delay(1000);
}
