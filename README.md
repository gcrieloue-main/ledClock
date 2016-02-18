# LedClockServer

Features :
* display clock on LED display
* listen REST commands

REST api :
* /raw/:command
* /brightness/:value
* /mode/:mode
* /preset/:preset
* /color/:r/:g/:b
* /color2/:r/:g/:b
* /backgroundColor/:r/:g/:b
* /text/:content/:repetition/:title
* /text/:content/:repetition
* /text/:content
* /animate/:animation
* /countdown/:min
* /help/


Destination Hardware : 
* Raspberry pi (2)
* Adafruit 16*32 LED display

![General project architecture](https://raw.githubusercontent.com/hellsdark/ledClock/master/images/ledClockArchitecture.png)
