#!/usr/bin/python
# -*- coding: utf-8 -*-

import serial

device = '/dev/ttyACM0' #this will have to be changed to the serial port you are using

try:
  print ("Trying..."),device 
  arduino = serial.Serial(device, 9600) 

except: 
  print ("Failed to connect on"),device  

data = arduino.readline()
pieces =data.split("\t")
getTemp = pieces[0]
phValue = pieces[1]
waterlevel = pieces[2]

with open("/path_to_directory/sensors.json","w+") as sensordata: sensordata.write('{"temperature": %s, "pH": %s, "niveau": %s}' % (pieces[0],pieces[1],pieces[2]))

print pieces[0],pieces[1],pieces[2]