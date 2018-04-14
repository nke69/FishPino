#FishPino

Aquarium Controller with Raspbeery Pi 3 and Arduino Uno : temperature heating and cooling, pH-controller, waterlevel indicator, controls a cooling-fan, heater and co2 system magnet valve, LCD Display for alert and views.

Explanation :


I used a 4 relay board, used 3 relay so you can do without the 4 leds.(those were for testing). Make sure the relay board has its own power-source (1x power adapter 12volt 1 amp,and 2x 7805 5 volt regulator. 1x 7805 for arduino and sensors and lcd, 1x 7805 for relay board.


Tips:


Sometimes lcd can get scrambled because of the relais switching or the switching of my TL lighting, 1 push on //reset to fix it. A small condenator (100nF-10uF,trial and error) directly soldered over the lcd power pins(VSS and VDD) might help. If lcd is crambled the controller still work properly,lcd is just for indication!
