#include <Arduino.h>
#include <Wire.h>

#define SLAVE_ADDRESS 0x04
int number = 0;
int state = 0;
float power = 0;

int relay_pin = 12;

#define GET_BYTES 1
#define ON 2
#define OFF 3

const float refVoltage = 4.44;
const float currentConstant1 = 0.1;// V:A
const float currentVoltageConstant1 = refVoltage/(1024*currentConstant1);

const int n_samples = 10; //samples per second
const int period = 1000/n_samples;
const int rms_samples = 40;


float volt_list[rms_samples] = {0};
float current_list[rms_samples]= {0};

int current_reading = 0;
int voltage_reading = 0;

float idle_hall = 0;
float current_constant = 0.66;

int cmd = 0;

int getHallAverage(byte analogPin, byte nSamples){//this function returns mean value o a current sensor
								long sum = 0;
								for(byte i=0; i<nSamples; i++) {
																sum = sum + analogRead(analogPin);
																delay(1);// makes sure ADC refreshes
								}
								sum = sum/nSamples;
								return sum;
}

float convert_to_volts(int voltage_reading){
        float ac_voltage = voltage_reading * (10.0/1023.0);
        ac_voltage = (ac_voltage - 5.0);
        ac_voltage = ac_voltage * 110.190;

        return ac_voltage;
}

float convert_to_amps(int current_reading){
        float ac_current = 0;
				ac_current = currentConstant1 * (current_reading - idle_hall);

        return ac_current;
}

void setup() {
        pinMode(13, OUTPUT);
        pinMode(12, OUTPUT);
        digitalWrite(12, LOW);

        Serial.begin(9600); // start serial for output
// initialize i2c as slave
        Wire.begin(SLAVE_ADDRESS);

// define callbacks for i2c communication
        Wire.onReceive(receiveData);
        Wire.onRequest(sendData);

        Serial.println("Ready!");
        idle_hall = getHallAverage(A0, 70);
        Serial.print("Idle hall");
        Serial.print(idle_hall);
}

void loop() {
        if((millis()%period)==0) {

                float rms_voltage = 0.0;
                float rms_current = 0.0;

                for(byte i = 0; i<rms_samples; i++) {
                        volt_list[i] =  convert_to_volts(analogRead(A0));
                        rms_voltage += volt_list[i]*volt_list[i];
                        // curretn measurment
                        current_list[i] = convert_to_amps(analogRead(A1));
                        rms_current = current_list[i]*current_list[i];
                }
                rms_voltage = rms_voltage/rms_samples;
                rms_voltage = sqrt(rms_voltage);

                rms_current = rms_current/rms_samples;
                rms_current = sqrt(rms_current);

                Serial.print(rms_voltage);
                Serial.print(rms_current);
								power = rms_current * rms_voltage;
								Serial.println(power);

        }

if(number == OFF){
	power = 0.0;
}

}

// callback for received data/
void receiveData(int byteCount){

        while(Wire.available()) {
                number = Wire.read();
                Serial.print("data received: ");
                Serial.println(number);

                if (number == ON) {
                        digitalWrite(relay_pin, HIGH);
                        Serial.println("TURNING RELAY ON CYKA");
                }
                else if (number == OFF) {
                        digitalWrite(relay_pin, LOW);
                        Serial.println("TURNING OFF-RELAY ON CYKA");
                }
        }
}

// callback for sending data
void sendData(){

//int pint = (int)power;
        long pint = static_cast<long>(power);
        if(number == GET_BYTES) {
                byte byte1 = pint >> 24; // gets msb
                byte byte2 = pint >> 16; // gets msb
                byte byte3 = pint >> 8; // gets msb
                byte byte4 = pint; // gets lsb
                Serial.println("---------------MAKING--------------");
                Serial.println(power);
                Serial.println(pint);
                Serial.println("Bytes below");
                Serial.println(byte1);
                Serial.println(byte2);
                Serial.println(byte3);
                Serial.println(byte4);
                Wire.write(byte1);
                Wire.write(byte2);
                Wire.write(byte3);
                Wire.write(byte4);

                Serial.println("---------------SENT--------------");

        }


}
