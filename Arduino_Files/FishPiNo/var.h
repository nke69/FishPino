/////////////////////////////////////#VAR///////////////////////////////////////////

int val, phset;
int state;
int utemp = 0, dtemp = 0, valup = 0, valdown = 0;

unsigned long int avgValue;  //Store the average value of the sensor feedback
#define Offset 1.80         //-0.45 offset pour la conversion du pH
#define Vref 3.50
int buf[10], temp; // tableau d'entiers de 10 valeurs et valeur tampon pour le triage
float b;

byte flame[8] = {B00100, B00100, B01110, B01010, B11011, B10001, B10001, B01110};       //icon for flame
byte smalflame[8] = {B00000, B00000, B00100, B00100, B01110, B01010, B10001, B01110};   //icon for small flame
byte thermometer[8] = {B00100, B01010, B01110, B01110, B11111, B11111, B11111, B01110}; //icon thermometer
byte celsius[8] = {B11100, B10100, B11100, B00000, B00000, B00000, B00000, B00000};     //icon celsius
byte tank[8] = {B01100, B00010, B00111, B01101, B10101, B10111, B10111, B00111};        //icon co2 tank
byte co2[8] = {B00001, B00100, B10001, B00100, B01010, B00001, B00010, B00001};         //icon c02 spray
