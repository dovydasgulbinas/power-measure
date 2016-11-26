#include <Arduino.h>
#include <Wire.h>

#define SLAVE_ADDRESS 0x04
int number = 0;
int state = 0;
float power = 12345.67;

int relay_pin = 12;

#define GET_BYTES 1
#define ON 2
#define OFF 3


void setup() {
pinMode(13, OUTPUT);
pinMode(12, OUTPUT);
digitalWrite(12, LOW);

Serial.begin(9600); // start serial for output
// initialize i2c as slave
Wire.begin(SLAVE_ADDRESS);

// define callbacks for i2c communication
Wire.onReceive(receiveData);
//Wire.onRequest(sendData);

Serial.println("Ready!");
}

void loop() {
delay(100);
}

// callback for received data/
void receiveData(int byteCount){

while(Wire.available()) {
number = Wire.read();
Serial.print("data received: ");
Serial.println(number);

if (number == ON){
digitalWrite(relay_pin, HIGH);
Serial.println("TURNING RELAY ON CYKA");
}
else if (number == OFF){
digitalWrite(relay_pin, LOW);
Serial.println("TURNING OFF-RELAY ON CYKA");
}
else if (number == GET_BYTES){
sendData();
}
}
}

// callback for sending data
void sendData(){

//int pint = (int)power;
long pint = static_cast<long>(power);

//if(number == GET_BYTES){
byte byte1 = pint >> 24; // gets msb
byte byte2 = pint >> 16; // gets msb
byte byte3 = pint >> 8; // gets msb
byte byte4 = pint; // gets lsb

Serial.println(power);
Serial.println(pint);
Serial.println("Bytes below");
// Serial.println(byte1);
// Serial.println(byte2);
// Serial.println(byte3);
// Serial.println(byte4);
Wire.write(byte1);
Wire.write(byte2);
Wire.write(byte3);
Wire.write(byte4);
//}


}
