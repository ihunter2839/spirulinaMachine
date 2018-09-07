#include <LiquidCrystal.h>
#include <OneWire.h>
#include <DallasTemperature.h>

/*
 * See details on back of LCD for pinout instructions
*/

#define btnUp 0
#define btnDown 1
#define btnRight 2
#define btnLeft 3
#define btnSelect 4
#define btnNone -1

//state variables
int buttonState = -1;
int deviceState = -1;

//turbidity variables
int turbAnalog;
float turbVolt;
float turbNTU;
int turbidityInputPin = A1;

//temperature variables
int tempPin = 2;
float tempF;
//construct the one wire parser
OneWire oneWire(2);
DallasTemperature sensors(&oneWire);

//flow variables
unsigned long oldTime;
byte flowInterrupt = 1;
int flowPin = 3;
volatile byte pulseCount = 0;
float flowRate = 0;
unsigned int flowMl = 0;
unsigned long totalMl = 0;
float calibrationFactor = 4.5;

//level variables
int seriesResistor = 4700;
int levelPin = A3;
float levelResistance;

//LCD variables
const int rs = 8, en = 9, d4 = 4, d5 = 5, d6 = 6, d7 = 7;
LiquidCrystal lcd(rs,en,d4,d5,d6,d7);

void setup() {
  Serial.begin(9600); 
    
  lcd.begin(16,2);
  lcd.setCursor(0,0);
  lcd.print("Spirulina");
  lcd.setCursor(0,1);
  lcd.print("Machine");

  sensors.begin();

  pinMode(flowPin, INPUT);
  digitalWrite(flowPin, HIGH);
  attachInterrupt(flowInterrupt, pulseCounter, FALLING);
}

void loop() {
  takeMeasurements();
  updateDeviceState();
  updateDisplay();
  delay(1000);
}

void clearLCD() {
  lcd.setCursor(0,0);
  lcd.print("                ");
  lcd.setCursor(0,1);
  lcd.print("                ");
}

void takeMeasurements() {
  //Turbidity measurement
  measureTurbidity();
  
  //Temperature measurement
  measureTemperature();

  //Flow measurement
  measureFlow();

  //Water level measurement
  measureLevel();
}

void measureLevel() { 
  levelResistance = analogRead(levelPin);
 
  Serial.print("Analog reading "); 
  Serial.println(levelResistance);
 
  // convert the value to resistance
  levelResistance = (1023 / levelResistance)  - 1;
  levelResistance = seriesResistor / levelResistance;
  Serial.print("Sensor resistance "); 
  Serial.println(levelResistance);
 }

void measureFlow() {
  if ((millis() - oldTime) > 1000) {
    detachInterrupt(flowInterrupt);

    flowRate = ((1000.0 / (millis() - oldTime)) * pulseCount) / calibrationFactor;

    oldTime = millis();

    //convert to l/s, then to ml
    flowMl = (flowRate / 60 ) * 1000;
    totalMl += flowMl;

    Serial.print("Flow rate: "); Serial.println(flowMl);
    Serial.print("Total flow: "); Serial.println(totalMl);

    pulseCount = 0;

    attachInterrupt(flowInterrupt, pulseCounter, FALLING);
  }
}

void pulseCounter() {
  pulseCount += 1;
}

void measureTemperature() {
  sensors.requestTemperatures(); 
  //the oneWire protocol can use many (up to 127) sensors on a single line
  //so, need to fetch the measurement from the first sensor on the line
  tempF = sensors.getTempFByIndex(0);
  Serial.print("Temperature (F): "); Serial.println(tempF);
  Serial.println("--------");
}

void measureTurbidity() {
  //Take an analog reading from pin the input pin 
  turbAnalog = analogRead(turbidityInputPin);
  turbVolt = turbAnalog * (5.0/ 1024.0); //Converstion from analog (0-1023) to voltage (0-5)
  //Convert to turbidity units
  //see https://www.dfrobot.com/wiki/index.php/Turbidity_sensor_SKU:_SEN0189
  // t = -1120.4v^2 + 5742.3v - 4352.9
  // where t = turbidity and v = voltage
  turbNTU = (-1120.4 * pow(turbVolt,2)) + (5742.3 * turbVolt) - 4352.9;
  Serial.print("Analog: "); Serial.println(turbAnalog);
  Serial.print("Voltage: "); Serial.println(turbVolt);
  Serial.print("Tubidity: "); Serial.println(turbNTU);
  Serial.println("------");
}

void updateDisplay() {
  if(deviceState == 0) {
    //up
    clearLCD();
    lcd.setCursor(0,0);
    lcd.print("Turbidity (NTU): ");
    lcd.setCursor(0,1);
    lcd.print(turbNTU);
  }
  if(deviceState == 1) {
    //down
    clearLCD();
    lcd.setCursor(0,0);
    lcd.print("Temperature (F): ");
    lcd.setCursor(0,1);
    lcd.print(tempF);
  }
  if(deviceState == 2) {
    //right
    clearLCD();
    lcd.setCursor(0,0);
    lcd.print("Flow (mL/s): ");
    lcd.setCursor(0,1);
    lcd.print(flowMl);
  }
  if(deviceState == 3) {
    //left
    clearLCD();
    lcd.setCursor(0,0);
    lcd.print("Water Level (ohms): ");
    lcd.setCursor(0,1);
    lcd.print(levelResistance);
  }
  if(deviceState == 4) {
    clearLCD();
    lcd.setCursor(0,0);
    lcd.print("Spirulina");
    lcd.setCursor(0,1);
    lcd.print("Machine");
  }
}

void updateDeviceState() {
   buttonState = getButtonState();
    //if the button has been pressed
    if(buttonState != -1) {
      //clear the opening text
      lcd.setCursor(0,0);
      lcd.print("                ");
      lcd.setCursor(0,1);
      lcd.print("                ");      
      //up
      if(buttonState == 0 ) {
        deviceState = 0;
      }
      //down
      if(buttonState == 1) {
        deviceState = 1;
      }
      //right
      if(buttonState == 2) {
        deviceState = 2;
      }
      //left
      if(buttonState == 3) {
        deviceState = 3;
      }
      //select
      if(buttonState == 4) { 
        deviceState = 4;
      }
    }
}

int getButtonState(){
  int analogIn = analogRead(0);
  if(analogIn < 50) return btnRight;
  if(analogIn < 250) return btnUp;
  if(analogIn < 450) return btnDown;
  if(analogIn < 650) return btnLeft;
  if(analogIn < 850) return btnSelect;

  return btnNone;
}

