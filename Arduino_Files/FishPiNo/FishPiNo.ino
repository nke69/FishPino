/*********************************************************************
   FishPiNo.ino
   Hardware platform   : Arduino Uno

   Description:
   Aquarium temperature (heating and cooling)) and pH-controller.
   Waterlevel indicator controls a cooling-fan, aquarium-heater and co2 system magnet valve.
   LCD Display for alert and views.

   Sensor pin:

   DIGITAL:
   LCD            : D2 - D3 - D4 - D5 - D11 - D12
   LEDUP          : D6  // led warning temp. upper limit or cooling Fan relais!
   WATERPIN       : D7  // floatswitch  for waterlevel
   LEDWATER       : D8  // warning led for water level
   DS18B20        : D9  // Signal pin on digital 9 temp sensor
   LEDPH          : D10 // led warning ph or relais for magnetic valve co2-system
   DHT22          : D13 // pin for DHT22

   ANALOG:
   dtempPin       : A1  //Lower temp.Limit pin with potmeter
   utempPin       : A2  //Upper temp.limit pin with potmeter
   SensorPin PH   : A3  //pH meter Analog output to Arduino Analog Input 3
   phsetPin       : A4  //set desired ph-value with potmeter
   HEATER         : A5  // led warning temp on  or RELAY FOR HEATER

   Free PIN:
   A0

   Author  :  nke69(nookie69@gmail.com)
   Version :  V1.0
   Date    :  31-03-2018

   Explanation :

   Controlling Aquarium temperature via heating and cooling relay, pH- controller and waterlevel indicator controls a cooling-fan,
   aquarium-heater and co2 system magnet valve.
   I used a 4 relay board, used 3 relay so you can do without the 3 leds.(those were for testing).
   Make sure the relay board has its own power-source (1x power adapter 12volt 1 amp,and 2x 7805 5 volt regulator.
   1x 7805 for arduino pro mini and sensors and and lcd, 1x 7805 for relay board.

   Parts list:

   1x arduino pro mini 16 mhz 5 volts with atmega 328.
   1x ph sensor  PH meterSKU: SEN0161, amplifier board included.
   1x DS18B20 one wire digital temperatur sensor, watersealed.
   1x 4 channel relais board.5 volts
   4x potentiometer 10 k for phset, maxtempset, mintempset and lcd-contrast.
   1x 16x2 lcd.
   1x power adapter 12 volt 1 amp.
   1x powerplug for housing to connect poweradapter to.
   2x 7805 volt regulator ,make a small pcb board with those two and the needed capacitors,to make 2 seperated 5 volts supplies, one for the relais board,one for the rest.
   1x led or blinkled for waterlevel warning indicator.
   1x 3 screw connector for temp sensor, 1x 2 screw connector for float switch sensor
   1x float sensor switch Water Level Liquid Sensor.
   2x build-on wall sockets ,both on 220 volts mains,here controlled by two relais,1 for heater and 1 for magnetvalve.
   1x pushbutton,connected to reset.

   Tips:

   Sometimes lcd can get scrambled because of the relais switching or the switching of my TL lighting, 1 push on //reset to fix it.
   a small condenator (100nF-10uF,trial and error) directly soldered over the lcd power pins(VSS and VDD) might help.
   if lcd is crambled the controller still work properly,lcd is just for indication!

 **********************************************************************/

#include <Adafruit_Sensor.h>
#include <LiquidCrystal.h>
#include <OneWire.h>
#include <SoftwareSerial.h>
#include "DHT.h"

/*DS18S20 Signal pin on digital 9 temp sensor , change to your digital pin number*/
int DS18S20_Pin = 9;

#define DHTPIN 13     // what pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302)
DHT dht(DHTPIN, DHTTYPE);

// pins use for LCD16x2
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

int val, phset;
int state;

int utemp = 0, dtemp = 0, valup = 0, valdown = 0;

//Temperature chip i/o
OneWire ds(DS18S20_Pin);     // ds18b20 on digital pin 9

const int dtempPin = A1;     //Lower temp.Limit pin with potmeter
const int utempPin = A2;     //Upper temp.limit pin with potmeter
#define phsetPin A4          //set desired ph-value with potmeter
const int waterPin = 7;      //floatswitch  for waterlevel
const int ledup = 6;         //led warning temp. upper limit or cooling Fan relais!
const int ledph = 10;        //led warning ph or relais for magnetic valve co2-system
const int ledwater = 8;      //warning led for water level
const int heat = A5;         //relais heater element
#define SensorPin A3         //pH meter Analog output to Arduino Analog Input 3
#define Offset -0.40           //set your offset ph sensor here!!! shorted then ph=7 ,here mine gives 6.8 so offset = 0.2
unsigned long int avgValue;  //Store the average value of the sensor feedback
float b;
int buf[10], temp;
byte flame[8] = {
  B00100, B00100, B01110, B01010, B11011, B10001, B10001, B01110
};       //icon for flame
byte smalflame[8] = {
  B00000, B00000, B00100, B00100, B01110, B01010, B10001, B01110
};   //icon for small flame
byte thermometer[8] = {
  B00100, B01010, B01110, B01110, B11111, B11111, B11111, B01110
}; //icon thermometer
byte celsius[8] = {
  B11100, B10100, B11100, B00000, B00000, B00000, B00000, B00000
};     //icon celsius
byte tank[8] = {
  B01100, B00010, B00111, B01101, B10101, B10111, B10111, B00111
};        //icon co2 tank
byte co2[8] = {
  B00001, B00100, B10001, B00100, B01010, B00001, B00010, B00001
};         //icon c02 spray

void setup()
{
  Serial.begin(9600);
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.createChar(1, flame);
  lcd.createChar(2, smalflame);
  lcd.createChar(3, thermometer);
  lcd.createChar(4, celsius);
  lcd.createChar(5, tank);
  lcd.createChar(6, co2);

  pinMode(ledup, OUTPUT);
  pinMode(ledph, OUTPUT);
  pinMode(heat, OUTPUT);
  pinMode(utempPin, INPUT);
  pinMode(dtempPin, INPUT);
  pinMode(phsetPin, INPUT);
  pinMode(waterPin, INPUT);
  pinMode(ledwater, OUTPUT);
  const int phsetPin = A4;
  int waterlevel;
  digitalWrite(ledup, LOW);  // relais off !! depends on relais board you have.
  digitalWrite(ledph, LOW);
  digitalWrite(heat, LOW);
  state == LOW ; // heat sign off
  dht.begin();
}

void loop()

{
  {
    float temperature = getTemp();                  // get temp from tempsensor
    utemp = analogRead(utempPin);                   // get max set temp from potmeter
    dtemp = analogRead(dtempPin);                   // get min set temp from potmeter
    float phset = analogRead(phsetPin);             // get desired ph value from potmeter

    utemp = map(utemp, 0, 1023, 19, 31);            // you can change 19 and 31 to your desired temp range for the set potmeter
    dtemp = map(dtemp, 0, 1023, 19, 31);            // idem
    phset = map(phset, 0.0, 1023.0, 10.0, 140.0);   // give the ph value
    phset = (phset / 20);                           // with 1 decimal
    for (int i = 0; i < 10; i++)                    //Get 10 sample value from the sensor for smooth the value
    {
      buf[i] = analogRead(SensorPin);
      delay(10);
    }
    for (int i = 0; i < 9; i++) //sort the analog from small to large
    {
      for (int j = i + 1; j < 10; j++)
      {
        if (buf[i] > buf[j])
        {
          temp = buf[i];
          buf[i] = buf[j];
          buf[j] = temp;
        }
      }
    }

    // Reading temperature or humidity takes about 250 milliseconds!
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
    float hum = dht.readHumidity();
    float temp = dht.readTemperature();

    avgValue = 0;
    for (int i = 2; i < 8; i++)                       //take the average value of 6 center sample
      avgValue += buf[i];
    float phValue = (float)avgValue * 5.0 / 1024 / 6; //convert the analog into millivolt
    phValue = 3.5 * phValue + Offset;                 //convert the millivolt into pH value

    int waterlevel;
    waterlevel = digitalRead(waterPin);

    if (waterlevel == HIGH)
    {

      digitalWrite(ledwater, HIGH);     //led on ,i used a blinkled
      lcd.setCursor(0, 1);
      lcd.print("^^^NIVEAU EAU^^^");    // water (niveau=)level  ,total of 16 characters incluiding tab
      delay(500);                       // change switchingspeed to your liking
      lcd.setCursor(0, 1);              // replace on same position
      lcd.print("____TROP BAS____");    // low  ,again total of 16 characters
      Serial.print(getTemp(), 1);
      Serial.print(" \t"); //tab
      delay(1000);
      Serial.print(phValue, 2);
      Serial.print(" \t"); //tab
      delay(1000);
      Serial.print(waterlevel);
      Serial.print(" \t"); //tab
      delay(1000);
      Serial.print(hum);
      Serial.print(" \t"); //tab
      delay(2000);
      Serial.println(temp);
      delay(3000);                       // again change to your liking
    }

    else

    {
      digitalWrite(ledwater, LOW); //led off
      lcd.setCursor(0, 0);
      lcd.print(char(3));
      lcd.setCursor(1, 0);
      lcd.print(getTemp(), 1);    // temp with 1 decimal
      lcd.setCursor(5, 0);
      lcd.print(char(4));
      lcd.setCursor(6, 0);
      lcd.print("C");
      lcd.setCursor(10, 0);
      lcd.print("pH:");
      lcd.setCursor(13, 0);
      lcd.print(phValue, 1);      // ph value with 1 decimal
      lcd.setCursor(7, 1);
      lcd.print(utemp);           //max set temp,no decimal
      lcd.setCursor(4, 1);
      lcd.print(" ");
      lcd.setCursor(0, 1);
      lcd.print("L~");            // L from Low set temp with arrow right
      lcd.setCursor(2, 1);
      lcd.print(dtemp);           //min set temp ,no decimal
      lcd.setCursor(12, 1);
      lcd.print("~");             //this sign gives on my lcd an arrow to the right
      lcd.setCursor(5, 1);
      lcd.print("H~");            //H from High set temp with arrow right
      lcd.setCursor(13, 1);
      lcd.print(phset, 1);        //ph set value with 1 decimal
      lcd.setCursor(9, 1);
      lcd.print(" ");
      lcd.setCursor(10, 1);
      lcd.print(char(5));
      delay(3000);               // Rafraichissement des donnees
      Serial.print(getTemp(), 1);
      Serial.print(" \t"); //tab
      delay(1000);
      Serial.print(phValue, 2);
      Serial.print(" \t"); //tab
      delay(1000);
      Serial.print(waterlevel);
      Serial.print(" \t"); //tab
      delay(1000);
      
      /*
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Humidite : ");
      lcd.setCursor(11, 0);
      lcd.print(hum);
      lcd.setCursor(0, 1);
      lcd.print("Temp.air : ");
      lcd.setCursor(11, 1);
      lcd.print(temp, 2);
      delay(3000);
      */

      // check if returns are valid, if they are NaN (not a number) then something went wrong!

      if (isnan(temp) || isnan(hum)) {
        Serial.println ("Lecture DHT impossible !");
      }

      else {
        Serial.print(hum);
        Serial.print(" \t"); //tab
        delay(2000);
        Serial.println(temp);
      }

      state = digitalRead(heat); // read the state of the heater-relais(when reset,the heatersign would turn on while heater is off.This gives the right indication.)
    }

    if (getTemp() > dtemp + (utemp - dtemp)) //temp higher than max set temp
    {
      digitalWrite(ledup, HIGH); //cooling fan on
      delay(10);
    }

    if (getTemp() <= dtemp)  //temp lower or equal than min set temp
    {
      digitalWrite(ledup, LOW); //cooling fan off
      delay(10);
    }

    if (getTemp() < dtemp - 0.4) // temp lower than 0.1 degrees-celsius below min set temp
    {
      digitalWrite(heat, HIGH);   // heater on
      delay(5);
      state = digitalRead(heat);
    }

    if (getTemp() >= dtemp + 0.3)  //temp higher than 0.2 degrees-celsius above min set temp
    {
      digitalWrite(heat, LOW);    //heater off
      delay(10);
      state = digitalRead(heat);
    }

    if (state == LOW)

    {
      lcd.setCursor(7, 0);
      lcd.print(char(1));
      lcd.setCursor(8, 0);
      lcd.print(char(2));
      delay(300);           // change sign
      lcd.setCursor(7, 0);
      lcd.print(char(2));   //  sign blink effect
      lcd.setCursor(8, 0);
      lcd.print(char(1));
      lcd.setCursor(9, 0);
      lcd.print(" ");
      delay(200);
    }

    if (state == HIGH)
    {
      lcd.setCursor(7, 0);
      lcd.print("   ");   // no sign heater
    }

    if (phValue > phset + 0.05)  // offset 0.05  pH is allowed to rise above the set pH value before before switching on co2 or led
    {
      digitalWrite(ledph, LOW);  // magnetvalve co2 open,(or led on in breadboard)
      delay(10);
      lcd.setCursor(11, 1);
      lcd.print(char(6));
      delay(5);
    }

    else if (phValue <= phset)
    {
      digitalWrite(ledph, HIGH); //magnetvalve co2 closed, (or led off in breadboard)
      delay(10);
      lcd.setCursor(11, 1);
      lcd.print(" ");
      delay(5);
    }
  }
}

float getTemp() {
  //returns the temperature from one DS18S20 in DEG Celsius
  byte data[12];
  byte addr[8];

  if ( !ds.search(addr)) {
    //no more sensors on chain, reset search
    ds.reset_search();
    return -1000;
  }

  if ( OneWire::crc8( addr, 7) != addr[7]) {
    //Serial.println("CRC is not valid!");
    return -1000;
  }

  if ( addr[0] != 0x10 && addr[0] != 0x28) {
    //Serial.print("Device is not recognized");
    return -1000;
  }

  ds.reset();
  ds.select(addr);
  ds.write(0x44, 1); // start conversion, with parasite power on at the end

  byte present = ds.reset();
  ds.select(addr);
  ds.write(0xBE); // Read Scratchpad

  for (int i = 0; i < 9; i++) { // we need 9 bytes
    data[i] = ds.read();
  }

  ds.reset_search();

  byte MSB = data[1];
  byte LSB = data[0];

  float tempRead = ((MSB << 8) | LSB); //using two's compliment
  float TemperatureSum = tempRead / 16;

  return TemperatureSum;
}


