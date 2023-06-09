#include <LSTM.h>

const int CHARGE_PIN = 2;           // Pin used to charge the capacitor
const int THRESHOLD = 512;         // Threshold value for pulse count
const int CAPACITANCE = 100;       // Capacitance of known capacitor in uF
const int AVERAGE_READINGS = 10;   // Number of analog readings to average
const int VOLTAGE_PIN = A0;        // Analog pin used to measure supply voltage
const int TRIGGER_PIN = 3;         // Pin used to trigger new resistance measurement
const int VOLTAGE_DIVIDER = 10;    // Voltage divider used to measure supply voltage
const float INTERNAL_REF_VOLTAGE = 1.1;  // Internal reference voltage of Arduino

uint32_t pulseCount = 0;                // Pulse count for current resistance measurement
float supplyVoltage = 0;           // Supply voltage for current resistance measurement
float resistance = 0;              // Resistance of unknown resistor
float predictedError = 0;          // Predicted error of LSTM
float measuredError = 0;           // Measured error of LSTM

LSTM lstm(5, 5, 1);                // LSTM with 5 input neurons, 5 hidden neurons, and 1 output neuron

void setup() {
  pinMode(CHARGE_PIN, OUTPUT);
  pinMode(TRIGGER_PIN, INPUT_PULLUP);
  Serial.begin(9600);
}

void loop() {
  if (digitalRead(TRIGGER_PIN) == LOW) {
    // Trigger new resistance measurement
    lstm.reset();  // Reset LSTM state
    pulseCount = 0;
    }
    supplyVoltage = measureSupplyVoltage();
    pulseCount = chargeCapacitor(CHARGE_PIN, THRESHOLD, CAPACITANCE);
    float analogInput = measureAnalogInput();
    digitalWrite(chargePin, LOW); // Begin next charging pulse
    resistance = calculateResistance(CAPACITANCE, pulseCount);
    float inputs[] = {analogInput, (float)pulseCount, supplyVoltage};
    // Use zero for software trigger input to avoid interference from previous measurement
    float target[] = {resistance};
    lstm.train(inputs, target);
    predictedError = lstm.predict(inputs);
    measuredError = resistance - predictedError;
    Serial.print("Resistance: ");
    Serial.print(resistance);
    Serial.print(", Predicted error: ");
    Serial.print(predictedError);
    Serial.print(", Measured error: ");
    Serial.println(measuredError);
}

float measureAnalogInput() {
  // Measure analog input and return the result
  int analogValue = analogRead(A0);
  return analogValue * INTERNAL_REF_VOLTAGE / 1023.0;
}

float measureSupplyVoltage() {
  // Measure supply voltage using voltage divider and internal reference voltage
  analogReference(INTERNAL);
  int analogValue = analogRead(VOLTAGE_PIN);
  analogReference(DEFAULT);
  return analogValue * INTERNAL_REF_VOLTAGE * VOLTAGE_DIVIDER / 1023.0;
}


uint32_t chargeCapacitor(int chargePin, int threshold, int capacitance) {
  pinMode(chargePin, OUTPUT); // Set charge pin as output
  digitalWrite(chargePin, LOW); // Begin charging pulse
  int pulseCount = 0;
  unsigned long startTime = millis(); // Record start time
  while(analogRead(chargePin) < threshold) {
    digitalWrite(chargePin, HIGH); // End charging pulse
    delayMicroseconds(10); // Wait for a short time
    //digitalWrite(chargePin, LOW); // to disable pullup 
    pinMode(chargePin, INPUT); // End charging pulse by setting charge pin as input
    pulseCount++;
  }
//digitalWrite(chargePin, LOW); // Begin next charging pulse
  unsigned long endTime = millis(); // Record end time
  float chargeTime = (endTime - startTime) * 1000.0 / pulseCount; // Calculate time per pulse
  //float resistance = chargeTime / (capacitance * 1000.0); // Calculate resistance
  return pulseCount;
}


float calculateResistance(int capacitance, int pulseCount) {
// Calculate resistance of unknown resistor from capacitance, pulse count, and supply voltage
float timeConstant = capacitance * pulseCount * 5.0 / 1023.0; // Time constant = RC = capacitance * resistance
return timeConstant / (capacitance * 5.0 / 1023.0 - timeConstant) * supplyVoltage; // Resistance = time constant / (capacitance - time constant) * supply voltage
}
