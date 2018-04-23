/////////////////////////////////////#VAR///////////////////////////////////////////

int val, phset;
int state;
int utemp = 0, dtemp = 0, valup = 0, valdown = 0;

unsigned long int avgValue; //Store the average value of the sensor feedback
#define Offset 1.80         //offset pH from sensor in calibration
int buf[10], temp;          // tableau d'entiers de 10 valeurs et valeur tampon pour le triage
float b;

const float closedVoltage = 5.00;                //assumed value of voltage (replace with vcc)
const float voltageRange = closedVoltage / 1024; //Used to map the analog reading to the voltage of the circuit.
const float resistor = 10000.00;                  //size of resistor on the ground line
int conductReading = 0;                        //reading from A0 pin
float conductVoltage = 0.00;                  //calculated voltage from reading
float resistance = 0.00;                    //calculated resistance of water (ohms)
float area = 0.0002;                          //area of the all four sides of the electrodes in square meters
float length = 0.01;                        //distance between the electrodes in meters
float resistivity = 0.0;                    //resistivity of fluid
float conductivity = 0.00;                  //calculated conductivity of water (micro-siemens/m)

byte flame[8] = {B00100, B00100, B01110, B01010, B11011, B10001, B10001, B01110};       //icon for flame
byte smalflame[8] = {B00000, B00000, B00100, B00100, B01110, B01010, B10001, B01110};   //icon for small flame
byte thermometer[8] = {B00100, B01010, B01110, B01110, B11111, B11111, B11111, B01110}; //icon thermometer
byte celsius[8] = {B11100, B10100, B11100, B00000, B00000, B00000, B00000, B00000};     //icon celsius
byte tank[8] = {B01100, B00010, B00111, B01101, B10101, B10111, B10111, B00111};        //icon co2 tank
byte co2[8] = {B00001, B00100, B10001, B00100, B01010, B00001, B00010, B00001};         //icon c02 spray
