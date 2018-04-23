///////////////////////////////////////#PIN/////////////////////////////////////////

/*
  Free Pin :
  D13
*/

// pins use for LCD16x2
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

int ECPin = A0;
const int dtempPin = A1;     //Lower temp.Limit pin with potmeter
const int utempPin = A2;     //Upper temp.limit pin with potmeter
#define SensorPin A3         //valeur analogique du pH meter sur la pin analogique 3
const int phsetPin = A4;     //Connect the sensor's Po output to analogue pin 4
#define phsetPin A4          //set desired ph-value with potmeter
const int heat = A5;         //relais heater element

const int ledup = 6;         //led warning temp. upper limit or cooling Fan relais!
const int waterPin = 7;      //floatswitch  for waterlevel
const int ledwater = 8;      //warning led for water level
int DS18S20_Pin = 9;         //Connect the sensor's T° output to digital pin 9
//Temperature chip i/o
OneWire ds(DS18S20_Pin);     // ds18b20 on digital pin 9
const int ledph = 10;        //led warning ph or relais for magnetic valve co2-system
