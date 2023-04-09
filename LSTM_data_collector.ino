// Define pins
const int chargePin = A0; // Charging pin connected to analog input
const int referenceResistor = 10000; // Resistance of reference resistor in ohms
const int threshold = 800; // Analog input threshold for capacitor voltage in millivolts

// Define variables
int capacitance = 100; // Capacitance of unknown capacitor in microfarads
int pulseCount = 0; // Number of pulses required to charge the capacitor above the threshold
float voltage = 5.0; // Supply voltage in volts
float supplyVoltage = 0.0; // Measured supply voltage in volts
float resistance = 0.0; // Resistance of unknown resistor in ohms
float analogReading = 0.0; // Raw analog reading
float averageReading = 0.0; // Average of 10 analog readings
bool newMeasurement = false; // Flag indicating a new resistance measurement is needed

void setup() {
  Serial.begin(9600); // Initialize serial communication
}

void loop() {
  // Measure supply voltage
  supplyVoltage = (analogRead(A7) * voltage / 1023.0) * 10.0; // Measure supply voltage with 1:10 voltage divider
  analogReference(DEFAULT); // Switch back to Vcc reference
  
  // Wait for new measurement trigger
  while (!newMeasurement) {
    delay(100);
  }
  newMeasurement = false;
  
  // Measure analog input
  analogReading = analogRead(chargePin) * voltage / 1023.0; // Convert raw analog reading to millivolts
  
  // Average 10 analog readings
  for (int i = 0; i < 10; i++) {
    averageReading += analogRead(chargePin) * voltage / 1023.0;
  }
  averageReading /= 10.0; // Calculate average reading
  
  // Charge capacitor and count pulses
  pulseCount = chargeCapacitor(chargePin, threshold, capacitance);
  
  // Calculate resistance
  float chargeTime = pulseCount * 10.0; // Calculate charge time in microseconds
  float current = voltage / referenceResistor; // Calculate charging current in amperes
  float charge = current * chargeTime / 1000000.0; // Calculate charge in coulombs
  resistance = (charge * voltage) / ((analogReading / 1000.0) * (voltage - supplyVoltage)); // Calculate resistance in ohms
  
  // Print results
  Serial.print("Analog reading: ");
  Serial.print(analogReading, 2);
  Serial.print(" mV, ");
  Serial.print("Average reading: ");
  Serial.print(averageReading, 2);
  Serial.print(" mV, ");
  Serial.print("Pulse count: ");
  Serial.print(pulseCount);
  Serial.print(", ");
  Serial.print("Supply voltage: ");
  Serial.print(supplyVoltage, 2);
  Serial.print(" V, ");
  Serial.print("Resistance: ");
  Serial.print(resistance, 2);
  Serial.println(" ohms");
  
  // Send data over serial console
  Serial.print(analogReading);
  Serial.print(",");
  Serial.print(averageReading);
  Serial.print(",");
  Serial.print(pulseCount);
  Serial.print(",");
  Serial.print(supplyVoltage);
  Serial.print(",");
  Serial.println(resistance);
}

// Charge the capacitor with discrete pulses and count how many pulses are needed to charge it above the threshold
int chargeCapacitor(int pin, int threshold,int capacitance) {
pinMode(pin, OUTPUT); // Set charging pin as output

// Discharge capacitor
digitalWrite(pin, LOW);
delay(10);
// Charge capacitor with pulses
int pulseCount = 0;
while (true) {
digitalWrite(pin, HIGH);
delayMicroseconds(10);
digitalWrite(pin, LOW);
delayMicroseconds(10);
pulseCount++;
if (analogRead(pin) > threshold) { // Check if capacitor voltage is above threshold
pinMode(pin, INPUT); // Set charging pin as input to stop the pulse
break;
}
}

return pulseCount;
}

// Trigger new resistance measurement on serial input
void serialEvent() {
if (Serial.available() > 0) {
char input = Serial.read();
if (input == 'm') {
newMeasurement = true;
}
}
}
