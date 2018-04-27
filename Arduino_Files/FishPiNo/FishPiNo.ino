/***********************************************************************
   FishPiNo.ino
   Hardware platform   : Arduino Uno
   Author  :  nke69
   Version :  V1.0
   Date    :  19/04/2018


   Table of contents:
   Libraries                                 - [#LIBRARIES]
   Global Variables                          - [#var.h]
   Pins                                      - [#pin.h]
   Methods                                   - [#method.h]
   Setup Function                            - [#SETUP]
   Start Of Main Loop                        - [#LOOP]
 **********************************************************************/

///////////////////////////////////#LIBRARIES///////////////////////////////////////

#include <Adafruit_Sensor.h>
#include <LiquidCrystal.h>
#include <OneWire.h>
#include <SoftwareSerial.h>
#include "var.h"
#include "pin.h"
#include "method.h"


/////////////////////////////////////#SETUP/////////////////////////////////////////

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
//  pinMode(ledph, OUTPUT);
  pinMode(heat, OUTPUT);
  pinMode(utempPin, INPUT);
  pinMode(dtempPin, INPUT);
  pinMode(phsetPin, INPUT);
  pinMode(waterPin, INPUT);
  pinMode(ledwater, OUTPUT);
  pinMode(voltageFlipPin1, OUTPUT);
  pinMode(voltageFlipPin2, OUTPUT);
  pinMode(sensorPin, INPUT);

  int waterlevel;
  digitalWrite(ledup, LOW);   // relais off !! depends on relais board you have.
//  digitalWrite(ledph, LOW);
  digitalWrite(heat, LOW);
  state == LOW ;              // heat sign off
}

//Calcul de la conductivité
void setSensorPolarity(boolean flip) {
  if (flip) {
    digitalWrite(voltageFlipPin1, HIGH);
    digitalWrite(voltageFlipPin2, LOW);
  } else {
    digitalWrite(voltageFlipPin1, LOW);
    digitalWrite(voltageFlipPin2, HIGH);
  }
}


/////////////////////////////////////#LOOP//////////////////////////////////////////

void loop()

{
  {
    float temperature = getTemp();                  // get temp from tempsensor
    utemp = analogRead(utempPin);                   // get max set temp from potmeter
    dtemp = analogRead(dtempPin);                   // get min set temp from potmeter
    float phset = analogRead(phsetPin);             // get desired ph value from potmeter

    utemp = map(utemp, 0, 1023, 22, 28);            // you can change 19 and 31 to your desired temp range for the set potmeter
    dtemp = map(dtemp, 0, 1023, 22, 28);            // idem

    phset = map(phset, 0, 1023, 60, 90);            // give the ph value for a range 6 to 9 pH
    phset = (phset / 10);                           // with 1 decimal

    for (int i = 0; i < 10; i++)                    //Get 10 sample value from the sensor for smooth the value
    {
      buf[i] = 1024 - analogRead(SensorPin);
      delay(10);
    }
    for (int i = 0; i < 9; i++)                     //sort the analog from small to large
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

    // calcul de la moyenne des mesures en ne prenant pas les deux plus petites ni les deux plus grandes
    avgValue = 0;
    for (int i = 2; i < 12; i++)                       //take the average value of 10 center sample
      avgValue += buf[i];
    avgValue = avgValue / 10;                          // cos' 10 mesures

    //conversion de la mesure
    //https://books.google.be/books?id=YDhRDwAAQBAJ&pg=PT658&lpg=PT658&dq=pHValue+%3D+3.5*voltage%2BOffset;&source=bl&ots=UYqLUOR3Y8&sig=-UURw-qU26oiAh84YzvhPay1iKU&hl=fr&sa=X&ved=2ahUKEwi1r9Xp3MvaAhUuMuwKHb-mAhsQ6AEwA3oECAAQXg#v=onepage&q=pHValue%20%3D%203.5*voltage%2BOffset%3B&f=false

    float voltage = (((float)avgValue) / 10.0) * (5.0 / 1024.0);              //convert the analog into millivolt
    float phValue = voltage * (-1.33) + 7.0;                                  //convert the millivolt into pH value
    //float adjustedph = phValue * 1.627 - 2.499;                               //linear formula
    float adjustedph = (-0.01796 * voltage + 1.8056) * phValue - 2.94;      //or quadratic formula
    adjustedph = phValue + Offset;                                            //add Offset if needed in var.h
    phValue = adjustedph;                                                     //convert into pH value
    /*
        //Ancienne formule pour le pH
        float phValue=(float)avgValue*5.0/1024; //convert the analog into millivolt
        phValue=3.5*phValue+Offset;                      //convert the millivolt into pH value
    */

    //Calcul niveau eau
    int waterlevel;
    waterlevel = digitalRead(waterPin);
    //Calcul niveau eau

    //Calcul de la conductivité
    setSensorPolarity(true);
    delay(flipTimer);
    int val1 = analogRead(sensorPin);
    delay(flipTimer);
    setSensorPolarity(false);
    delay(flipTimer);
    // invert the reading
    int val2 = 1023 - analogRead(sensorPin);
    //
    //reportLevels(val1, val2);

    int avg = (val1 + val2) / 2;

    const float resistorValue = 100000.0;
    float voltage2 = avg * (5.0 / 1023.0); //calculation of relative conductivity
    float resistance = ((5.00 * resistorValue) / voltage2) - resistorValue;
    float Siemens = 1.0 / (resistance / 100000000);
    //Calcul de la conductivité


    if (waterlevel == HIGH)
    {

      digitalWrite(ledwater, HIGH);     //led on ,i used a blinkled
      lcd.setCursor(0, 1);
      lcd.print(F("^^^NIVEAU EAU^^^")); // water (niveau=)level  ,total of 16 characters incluiding tab
      delay(500);                       // change switchingspeed to your liking
      lcd.setCursor(0, 1);              // replace on same position
      lcd.print(F("____TROP BAS____")); // low  ,again total of 16 characters
      Serial.print(getTemp(), 1);
      Serial.print(F(" \t")); //tab
      delay(1500);
      Serial.print(phValue, 2);
      Serial.print(F(" \t")); //tab
      delay(1500);
      Serial.print(waterlevel);
      Serial.print(F(" \t")); //tab
      delay(1500);
      Serial.println(avg); //Calcul de la conductivité
      //if (voltage2 > 0.5) Serial.println(F("0"));
      delay(5000);                      // again change to your liking
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
      lcd.print(F("pH:"));
      lcd.setCursor(13, 0);
      lcd.print(phValue, 1);      // ph value with 1 decimal
      lcd.setCursor(7, 1);
      lcd.print(utemp);           //max set temp,no decimal
      lcd.setCursor(4, 1);
      lcd.print(F(" "));
      lcd.setCursor(0, 1);
      lcd.print(F("L~"));         // L from Low set temp with arrow right
      lcd.setCursor(2, 1);
      lcd.print(dtemp);           //min set temp ,no decimal
      lcd.setCursor(12, 1);
      lcd.print(F("~"));          //this sign gives on my lcd an arrow to the right
      lcd.setCursor(5, 1);
      lcd.print(F("H~"));         //H from High set temp with arrow right
      lcd.setCursor(13, 1);
      lcd.print(phset, 1);        //ph set value with 1 decimal
      lcd.setCursor(9, 1);
      lcd.print(F(" "));
      lcd.setCursor(10, 1);
      lcd.print(char(5));
      //delay(3000);                // Rafraichissement des donnees
      Serial.print(getTemp(), 1);
      Serial.print(F(" \t"));     //tab
      delay(1500);
      Serial.print(phValue, 2);
      Serial.print(F(" \t"));     //tab
      delay(1500);
      Serial.print(waterlevel);
      Serial.print(F(" \t")); //tab
      delay(1500);
      Serial.println(Siemens); //Calcul de la conductivité
      //if (voltage2 > 0.5) Serial.println(F("0"));
      delay(5000);                      // again change to your liking
      state = digitalRead(heat);  // read the state of the heater-relais(when reset,the heatersign would turn on while heater is off.This gives the right indication.)
    }

    if (getTemp() > dtemp + (utemp - dtemp)) //temp higher than max set temp
    {
      digitalWrite(ledup, HIGH);  //cooling fan on
      delay(10);
    }

    if (getTemp() <= dtemp)       //temp lower or equal than min set temp
    {
      digitalWrite(ledup, LOW);   //cooling fan off
      delay(10);
    }

    if (getTemp() < dtemp - 0.1)  // temp lower than 0.1 degrees-celsius below min set temp
    {
      digitalWrite(heat, HIGH);   // heater on
      delay(5);
      state = digitalRead(heat);
    }

    if (getTemp() >= dtemp + 0.2) //temp higher than 0.2 degrees-celsius above min set temp
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
      delay(300);                 // change sign
      lcd.setCursor(7, 0);
      lcd.print(char(2));         //  sign blink effect
      lcd.setCursor(8, 0);
      lcd.print(char(1));
      lcd.setCursor(9, 0);
      lcd.print(F(" "));
      delay(200);
    }

    if (state == HIGH)
    {
      lcd.setCursor(7, 0);
      lcd.print(F("   "));        // no sign heater
    }

    if (phValue > phset + 0.05)   // offset 0.05  pH is allowed to rise above the set pH value before before switching on co2 or led
    {
      //digitalWrite(ledph, LOW);   // magnetvalve co2 open,(or led on in breadboard)
      delay(10);
      lcd.setCursor(11, 1);
      lcd.print(char(6));
      delay(5);
    }

    else if (phValue <= phset)
    {
      //digitalWrite(ledph, HIGH);  //magnetvalve co2 closed, (or led off in breadboard)
      delay(10);
      lcd.setCursor(11, 1);
      lcd.print(F(" "));
      delay(5);
    }
  }
}
