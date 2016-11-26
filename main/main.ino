#include <Arduino.h>


#include <Wire.h>
#include  <TimerOne.h>
//#########################
#define socketPin_B0 (3) // Socket pin BASIC0
#define buttonPin_0 (7)//

int currentPin_0 = A0;
int currentPin_1 = A1;
int currentPin_2 = A2;


///NON pin constants
#define OFF (0)
#define ON  (1)
#define I2C_ID (4)
////

///POWER VARIABLES
const byte minimumPower = 17;
const int nSamples = 40;
const float refVoltage = 4.44;
const float currentConstant1 = 0.1;// V:A
const float currentConstant2 = 0.066;// V:A
const float currentVoltageConstant1 = refVoltage/(1024*currentConstant1);
const float currentVoltageConstant2 = refVoltage/(1024*currentConstant2);
volatile const int timerFrequency = 13333; // 1S/75uS the frequency of how often function is called
volatile const int callMeasurment = timerFrequency/nSamples; // Rouding erros might occur Assuming that timer calls nSamples numter of samples per second
volatile int callCounter = 0;
float RMSVoltage = 230;
////

///Communication VARIABLES
const byte rozeteON = 4;
const byte rozeteOFF = 5;
const byte readPower_MSB = 6;
const byte readPower_LSB = 7;
const byte getStates = 8;
const byte beginDemo = 9;
////

////misc VARIABLES
byte lastCMD = 0;
byte lastByte2 = 0;
byte stateByte = 0;
boolean demo = false;
boolean ONCycle = false; // else means of cycle must be initiated
byte demoCounter =0;
///

struct buttonStruct{
	uint8_t cState : 1;
	uint8_t pState : 1;
	uint8_t buttPin : 4;
} button[1];

struct powerStruct{
	uint8_t MSBPower : 8;
	uint8_t LSBPower : 8;
	int power : 16;
	float RMSCurrent;
	float currentSample[nSamples];
	int analogPin; //the pin to which analog input is connected
	int analogMean; //used for storing analog value of the sensor
	float currentVoltageConst;
}powerStr[1];


///Interupt based functions

void dim_check() { //Timer interupput function called every 75uS or in variable terms every freqStep
	///TIMING FOR measurment
	if(callCounter<nSamples){
		for(byte i = 0; i<3;i++){
			powerStr[i].currentSample[callCounter]=analogRead(powerStr[i].analogPin)-powerStr[i].analogMean;//reads the current value and subtracts the mean middle value
			powerStr[i].currentSample[callCounter]= powerStr[i].currentSample[callCounter]*powerStr[i].currentSample[callCounter];// get the Square of the measurment!
		}//for1
		callCounter++;
	}//if6

	else{//40 samples has been received calculating power
		for(byte n =0;n<3;n++){//this loop goes trhough 3 of the analog current sensors
			for(byte s = 0; s<nSamples;s++){ //summs all of the samples to create RMS current
				powerStr[n].RMSCurrent+=powerStr[n].currentSample[s]; //sums all of the current samples to rms current varbiale s1^2+s2^2....
			}//for3
			powerStr[n].RMSCurrent=powerStr[n].RMSCurrent/nSamples; // devides by the number of samples present
			powerStr[n].RMSCurrent=sqrt(powerStr[n].RMSCurrent); //The final RMS current
			powerStr[n].power =(int)((powerStr[n].RMSCurrent*RMSVoltage*powerStr[n].currentVoltageConst)); // I_RMS * U_RMS * currentvoltageConstant
			if(powerStr[n].power<minimumPower)
			powerStr[n].power = 0;
		}//for2
		callCounter=0;
	}
}




void setup()
{
	pinMode(socketPin_B0, OUTPUT);
	pinMode(buttonPin_0, OUTPUT);
	pinMode(buttonLine, INPUT);

	for(byte i = socketPin_B0; i<=dimmerPin;i++){
		digitalWrite(i,LOW);  // pulls all outputs low!
	}//for1
	for(byte b = buttonPin_0; b <=buttonPin_2;b++){
		digitalWrite(b,HIGH);
	}//for2

	///Initializing structures
	button[0]= {0,0,buttonPin_0};
	powerStr[0] = {0,0,111,0,{0},currentPin_0,0,currentVoltageConstant1};
	////

//	attachInterrupt(0, zero_cross_detect, RISING);    // Attach an Interupt to Pin 2 (interupt 0) for Zero Cross Detection
	Timer1.initialize(freqStep);                      // Initialize TimerOne library for the freq we need
	Timer1.attachInterrupt(dim_check, freqStep);

	Wire.begin(I2C_ID); //               // join i2c bus with address #4
	Wire.onReceive(receiveEvent); // register event
	Wire.onRequest(requestEvent);
//	Serial.begin(9600);           // start serial for output
	delay(100);


	///Getting mean current
	for(byte c = 0; c<3;c++){
		powerStr[c].analogMean= getHallAverage(powerStr[c].analogPin,70);
	}
	////
	//DEFINE ALL FUCKING PINS AND SET THEM TO LOW!!!!
	delay(100);
}

void loop()
{


	if((millis()%99)==0){// Scanns button state every 0.1 sec // register changes
		//Serial.print("I_RMS0: ");
		//Serial.println(powerStr[0].RMSCurrent);
		//Serial.print("P_RMS0: ");
		//Serial.println(powerStr[0].power);

		for(int i = 0; i<3; i++){
			digitalWrite(button[i].buttPin,HIGH);
			button[i].cState = digitalRead(buttonLine); // Reads the current state of buttonLine
			if(button[i].cState != button[i].pState){ //Button was either pressed or released
				if(button[i].cState == 1){//Button has been pressed!
					//Serial.print ("Pressed: ");
					//Serial.println(i);
					if((int)bitRead(stateByte,i)==1){ //socket was ON and will be turned OFF!
						demo = false;
						ctrlSockets(i,OFF);
					}//if4
					else{ //socket was OFF and will be turned ON!
						demo = false;
						ctrlSockets(i,ON);
					}//else1.if4
				}//if3
			}//if2
			button[i].pState = button[i].cState; // Saves the last state for the next loop
			digitalWrite(button[i].buttPin,LOW); // Writes button pin low
		}//for1
	}//if1

}//loop

int getHallAverage(byte analogPin, byte nSamples){//this function returns mean value o a current sensor
	long sum = 0;
	for(byte i=0;i<nSamples;i++){
		sum = sum + analogRead(analogPin);
		delay(1);// makes sure ADC refreshes
	}
	sum = sum/nSamples;
	//Serial.println(sum);
	return sum;
}
void ctrlSockets(byte socketID, byte socketState) { //Assuming sockets are counted from 0; socketID is the id of a specific socket; socketState is on or off or dim ; statesByte is the refere
	//Assuming that socketState is either HIGH or LOW; HIGH means socket is ON; LOW means socket is OFF! //Function must be modified for button inverting states
	byte bitVal = 0;

	switch (socketID) {
	case 0:

		if(socketState == ON){
			bitWrite(stateByte, socketID,1);
			digitalWrite(socketPin_B0,HIGH);
		}
		else if (socketState == OFF){
			bitWrite(stateByte, socketID,0);
			digitalWrite(socketPin_B0,LOW);
		}
		//Serial.print("StateByte: = B");
		//Serial.println(stateByte, BIN);
		break;
		//////////////////
		} //Switch
}// ctrlSockets


// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void receiveEvent(int howMany)
{
	lastCMD = Wire.read();
	lastByte2 = Wire.read();

	switch (lastCMD) {

	case rozeteON:
		demo = false;
		ctrlSockets(lastByte2, ON);
		//Serial.print("rozeteON! = ");
		//Serial.println(lastByte2);
		//Serial.println("");
		break;

	case rozeteOFF:
		demo = false;
		ctrlSockets(lastByte2, OFF);
		//Serial.print("rozeteOFF! = ");
		//Serial.println(lastByte2);
		//Serial.println("");
		break;

	case readPower_MSB :
		//Serial.print("LB2= ");
		//Serial.println(lastByte2);
		/// convert to byte array;
		powerStr[lastByte2].MSBPower = powerStr[lastByte2].power >> 8; // we shift the power of the socket by 8 positions so that we could split it in to two bytes
		//Serial.print("MSB= ");
		//Serial.println(powerStr[lastByte2].MSBPower);
		break;

	case readPower_LSB:
		//Serial.print("LB2= ");
		//Serial.println(lastByte2);
		powerStr[lastByte2].LSBPower = powerStr[lastByte2].power;
		//Serial.print("LSB= ");
		//Serial.println(powerStr[lastByte2].MSBPower);
		break;

	case getStates:
		demo = false;
		//Serial.print("socket States = ");
		//Serial.println(stateByte, BIN);
		//Serial.println("");
		break;

	case beginDemo:
		demo = true;
demoCounter=0;
		//Serial.println("DEMO");
		break;
	default:
		//Serial.print("No Such command! = ");
		//Serial.println(lastByte2);
		//Serial.println("");
demo = false;
		// if nothing else matches, do the default
		// default is optional
	} //Switch

} //Event



void requestEvent( ) { //After receive event has been triggered MCU stores lastCMD which is used in I2C request so that data could be sent

	switch (lastCMD) {
	case readPower_MSB :
		//Serial.print("READPOWER =  ");
		Wire.write(powerStr[lastByte2].MSBPower);
		//Serial.println("");
		break;

	case readPower_LSB:
		//Serial.print("READPOWER = ");
		Wire.write(powerStr[lastByte2].LSBPower);
		//Serial.println("");
		break;

	case getStates:
		//Serial.print("statesPushed!");
		Wire.write(stateByte);
		//Serial.println("");
		break;

		default :
demo=false;
//Serial.println("NO CASE!");


	}//switch
}//requestEvent

void serialEvent() {
	int socket = 0;
	switch (Serial.read()) {

	case 'b': // Used for setting the brightness
		brightness = Serial.parseInt();
		//Serial.print("brightnes = ");
		//Serial.println(brightness);
		break;


	case 'd':
		demo = false;
		ctrlSockets(2, DIM);
		//Serial.println("Dimming!");
		break;

	case 'o':
		demo = false;
		ctrlSockets(2, OFF);
		//Serial.println("dim OFF!");
		break;


	case 'q': // turns on any socket
		demo = false;
		socket = Serial.parseInt();
		//digitalWrite(socketPin_B0,HIGH);
		ctrlSockets(socket, ON);
		//Serial.print("socket on = ");
		//Serial.println(socket);
		break;

	case 'w': // turns off any socket
		demo = false;
		socket = Serial.parseInt();
		ctrlSockets(socket, OFF);
		//Serial.print("socket off = ");
		//Serial.println(socket);
		break;

	case 'p': //present demo
demoCounter=0;
demo = true;
		//Serial.println("demoSerial!");
		break;

		default :
demo =false;
		//Serial.println("NO SUCH COMMAND!");
	}//switch

}
