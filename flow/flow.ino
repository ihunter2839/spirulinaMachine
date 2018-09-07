unsigned long oldTime; 

byte flowInterrupt = 1;
int flowPin = 3;

volatile byte pulseCount = 0;

float flowRate = 0;
unsigned int flowMl = 0;
unsigned long totalMl = 0;

float calibrationFactor = 4.5;

void setup() {
  Serial.begin(9600);
  
  pinMode(flowPin, INPUT);
  digitalWrite(flowPin, HIGH);

  attachInterrupt(flowInterrupt, pulseCounter, FALLING);
}

void loop() {
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

