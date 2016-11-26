#include <Arduino.h>
// the setup routine runs once when you press reset:
const float refVoltage = 4.44;
const float currentConstant1 = 0.1;// V:A
const float currentConstant2 = 0.066;// V:A
const float currentVoltageConstant1 = refVoltage/(1024*currentConstant1);
const float currentVoltageConstant2 = refVoltage/(1024*currentConstant2);

void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
}

// the loop routine runs over and over again forever:
void loop() {
  // read the input on analog pin 0:
  int sensorValue = analogRead(A0);
  int current_reading = analogRead(A1);
  // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):

	float ac_voltage = sensorValue * (10.0/1023.0);
 	ac_voltage = (ac_voltage - 5.0);
	ac_voltage = ac_voltage * 110.190;
	float current = current_reading*currentVoltageConstant1;
  // print out the value you read:
delay(750);
	Serial.print(" Voltage: ");
  Serial.print(ac_voltage);
	Serial.print(" Current: ");
	Serial.print(current);
		Serial.print(" Power: ");
	Serial.print(current * ac_voltage);
	Serial.print(" CS-val: ");
	Serial.println(sensorValue);
}
