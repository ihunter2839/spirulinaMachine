//Pin # 
int inputPin = A0;

void setup() {
  //Start serial communications. 
  Serial.begin(9600); 
}

void loop() {
  //Take an analog reading from pin the input pin 
  int analog = analogRead(inputPin);
  float voltage = analog * (5.0/ 1024.0); //Converstion from analog (0-1023) to voltage (0-5)
  //Convert to turbidity units
  //see https://www.dfrobot.com/wiki/index.php/Turbidity_sensor_SKU:_SEN0189
  // t = -1120.4v^2 + 5742.3v - 4352.9
  // where t = turbidity and v = voltage
  float turbidity = (-1120.4 * pow(voltage,2)) + (5742.3 * voltage) - 4352.9;
  Serial.print("Analog: "); Serial.println(analog);
  Serial.print("Voltage: "); Serial.println(voltage);
  Serial.print("Tubidity: "); Serial.println(turbidity);
  Serial.println("------");
  delay(500);
}
