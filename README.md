# sonoff-iota-cashier

1. Open Arduino IDE -> File -> Preferences -> Additional Boards Manager URLs

2. Enter the following link: http://arduino.esp8266.com/stable/package_esp8266com_index.json

3. Select Tools -> Board -> "ITEAD Sonoff"

4. The following libraries need to be included in Sketch -> Include library -> Manage libraries: ArduinoHttpClient, ArduinoJson

5. Clone the iota-arduino-client from https://github.com/francescolavra/arduino-iota-client into the libraries folder of Arduino IDE

6. Use a FTDI adapter to connect Sonoff to your computer

7. Select the USB port from Sketch -> Port

8. Select eeprom_clear from File -> Examples -> EEPROM, compile and upload to set EEPROM to 0. Only neccessary once.

9. Compile and upload sonoff-iota-cashier


# 