#define SERIESRESISTOR 4700

#define SENSORPIN A3

void setup() {
  Serial.begin(9600);
}

void loop() {
  float reading;
 
  reading = analogRead(SENSORPIN);
 
  Serial.print("Analog reading "); 
  Serial.println(reading);
 
  // convert the value to resistance
  reading = (1023 / reading)  - 1;
  reading = SERIESRESISTOR / reading;
  Serial.print("Sensor resistance "); 
  Serial.println(reading);
 
  delay(1000);
}
