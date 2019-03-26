# ESP32-FASTLED-OPC
An example of running an open pixel server on ESP32 with FastLed as the RGB strip engine.

# USE
1) Get a esp32 board and connect to usb
2) start the Arduino IDE with the esp32 SDK installed and open the ino file inside the esp32_opc folder
3) flash the program into the esp32_opc

# Connecting a RGB pixel strip/ring/matrix

Connect any supported RGB hardware to the esp32 and ensure you edit the line
#define PIN xx
To the pin of your choice. This pin is the data-in pin of the RGB peripheral

# Running the samples
Sample clients have been provided for Python/Nodejs/Processing

Just ensure that the OPC server IP in each of these examples is given the IP of your esp32_opc
which is running the esp32_opc.ino code in it.

Please feel free to open issues in case you need help with the code or you find bugs.  

My project takes the code from the repo
[https://github.com/ppelleti/esp-opc-server](https://github.com/ppelleti/esp-opc-server)
and extends it to use FastLED engine instead of adafruit's neopixel library.

# OPC implementation:
[https://github.com/zestyping/openpixelcontrol](https://github.com/zestyping/openpixelcontrol)

# Other Awesome Neopixel projects
1) [https://github.com/longears/pixelslinger](https://github.com/longears/pixelslinger)
2) [https://www.bhencke.com/pixelblaze](https://www.bhencke.com/pixelblaze)

# Version v1.0
---------------------------------
1) Initial code release
2) Added FastLED as the RGB engine. OPC client can control the RGS LED on strips/rings/matrixes
3) Added Arduino OTA capability to ensure OTA firmware upgrades.
4) added Wifi Manager to avoid storing the SSID name and password on the code.
