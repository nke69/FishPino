/***********************************************************************
 * FishPiNo.ino
 * Hardware platform   : Arduino Uno
 * Author  :  nke69
 * Version :  V1.0
 * Date    :  19/04/2018
 * 
 * 
 * Table of contents:
 * Libraries                                 - [#LIBRARIES]
 * Global Variables                          - [#var.h]
 * Pins                                      - [#pin.h]
 * Methods                                   - [#method.h]
 * Setup Function                            - [#SETUP]
 * Start Of Main Loop                        - [#LOOP]
 **********************************************************************/

///////////////////////////////////#LIBRARIES///////////////////////////////////////

#include <Firmata.h>
#include <Boards.h>
#include <ArduinoJson.h>
#include <Adafruit_Sensor.h>
#include <LiquidCrystal.h>
#include <OneWire.h>
#include <SoftwareSerial.h>
#include "var.h"
#include "pin.h"
#include "method.h"

#define DIM  10
int pwm =0;
String recu;

/////////////////////////////////////#SETUP/////////////////////////////////////////

void setup()
{
  Serial.begin(115200);
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
  
  
  
  
  
  pinMode(pin13,OUTPUT);
  
  pinMode(2,1);
  pinMode(3,1);
  pinMode(4,1);
  pinMode(A3,1);
  pinMode(A1,0);
  analogWrite(A3,1024);


  
  
  const int phsetPin = A4;
  int waterlevel;
  digitalWrite(ledup, LOW);   // relais off !! depends on relais board you have.
  digitalWrite(ledph, LOW);
  digitalWrite(heat, LOW);
  state == LOW ;              // heat sign off
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
      buf[i]=analogRead(SensorPin);
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

    avgValue=0;
    for(int i=2;i<8;i++)                      //take the average value of 6 center sample
      avgValue+=buf[i];
    float phValue=(float)avgValue*5.0/1024/6; //convert the analog into millivolt
    phValue=3.5*phValue+Offset;                      //convert the millivolt into pH value

    int waterlevel;
    waterlevel=digitalRead(waterPin);

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
      Serial.println(F(" \t")); //tab
      delay(5000);
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
      Serial.print("{\"temperature_raw\":");
      Serial.print(getTemp(), 1);
      delay(2000);
      //Serial.print(F(" \t"));     //tab
      Serial.print(",\"pH_raw\":");
      Serial.print(phValue, 2);
      delay(2000);
      //Serial.print(F(" \t"));     //tab
      Serial.print(",\"niveau_raw\":");
      Serial.print(waterlevel);
      //Serial.println(F(" \t")); //tab
      Serial.println("}");
      delay(2000);
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
  
  
  if(Serial.available()>0)
  {
    recu = Serial.readStringUntil('\n');
    // Ordre d'allumer la LED13
    if(recu=="L13") digitalWrite(pin13,HIGH);
    else 
    // Ordre d'éteindre la LED13
    if(recu == "l13") digitalWrite(pin13,LOW);
    else
    // Ordre de gradation de la LED10
    if((pwm=recu.toInt())<=100 && pwm>=0) analogWrite(DIM,map(pwm,0,100,0,255));   

    if(recu.substring(0,2)=="on")
      digitalWrite((recu.substring(2,4)).toInt(),1);
    else if(recu.substring(0,3)=="off")
      digitalWrite((recu.substring(3,5)).toInt(),0);
    else if(recu.substring(0,3)=="tog")
      digitalWrite((recu.substring(3,5)).toInt(),digitalRead((recu.substring(3,5)).toInt())?0:1);
    else if(recu.substring(0,3)=="pwm")
      analogWrite((recu.substring(3,5)).toInt(),(recu.substring(5,8)).toInt());   
   
    
  }
  int pot1 = analogRead(A1);
  int pot2 = analogRead(A2);
  
  Serial.print(pot1);
  Serial.print(";");
  Serial.print(pot2);
  Serial.print("\n");  

 
}

