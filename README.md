# LSTM_thermistor
LSTM predictor for measuring temperature 
by gpt3


In this code, the `loop()` function waits for the software trigger input (`TRIGGER_PIN`) to go low, indicating a new resistance measurement is requested. Once triggered, the code charges the capacitor with a series of pulses and counts the number of pulses required to exceed the threshold voltage (`THRESHOLD`). The pulse count is averaged over `AVERAGE_READINGS` readings to improve accuracy.

The code then calculates the resistance of the unknown resistor using the capacitance of the known capacitor (`CAPACITANCE`), the pulse count, and the supply voltage (`supplyVoltage`). It then trains the LSTM with inputs consisting of the analog input (`analogInput`), the average analog input over the number of readings (`analogInput / AVERAGE_READINGS`), the pulse count, the supply voltage, and a software trigger input set to zero to avoid interference from previous measurements.

The LSTM is then used to predict the error in the resistance calculation (`predictedError`) and the measured error is computed as the difference between the actual resistance and the predicted error (`measuredError`).

The `measureAnalogInput()` function measures the analog input on `A0` and returns the result, and the `measureSupplyVoltage()` function measures the supply voltage using a voltage divider and the internal reference voltage of the Arduino.

The `chargeCapacitor()` function charges the capacitor with discrete pulses until the threshold voltage is exceeded, and returns the number of pulses required. Finally, the `calculateResistance()` function calculates the resistance of the unknown resistor from the capacitance, pulse count, and supply voltage.

Overall, this code uses a combination of pulse counting, averaging, and LSTM prediction to improve the precision and accuracy of the resistance measurement.
