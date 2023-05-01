<h1>Be Alert with Smart Human Detection Technology!</h1>

## README

This system is an IoT device that senses motion and noise levels in a room, and sends data to a ThingSpeak channel. The data includes the number of times motion was detected, and the maximum noise level during a 60 second interval. This system can be used for home security, monitoring children or pets, or for noise level monitoring in classrooms or workplaces.

### Hardware Requirements
- ESP32 development board
- PIR motion sensor
- KY-037 sound sensor
- LED
- Breadboard
- Jumper wires

### Software Requirements
- Arduino IDE
- WiFi library for ESP32
- ThingSpeak library

### Configuration
In the Arduino IDE, the following configuration is required:
1. Set the board to ESP32 Dev Module.
2. Install the required libraries: WiFi and ThingSpeak.
3. Enter the WiFi network SSID and password in the code.
4. Enter the ThingSpeak channel ID, Write API key, and Read API key in the code.
5. Connect the hardware components to the ESP32 development board according to the pin assignments in the code.

### Operation
1. Power up the ESP32 development board.
2. The system will connect to the WiFi network and print the IP address to the Serial Monitor.
3. The system will calibrate the microphone by measuring the maximum value of the analog input during a 10 second interval.
4. The system will continuously monitor for motion and noise levels for 60 seconds.
5. At the end of each 60 second interval, the system will send data to the ThingSpeak channel, including the number of times motion was detected and the maximum noise level during the interval.
6. The LED will turn on briefly to indicate that data has been sent to ThingSpeak.


<h2>[Demonstration](https://youtu.be/HtsINyg1M0o)</h2>


<img src="https://github.com/no4paragon/CS549Iot-finalProject/blob/main/1.jpg" width="40%" height="40%">
<img src="https://github.com/no4paragon/CS549Iot-finalProject/blob/main/2.jpg" width="40%" height="40%">
<img src="https://github.com/no4paragon/CS549Iot-finalProject/blob/main/3.jpg" width="40%" height="40%">
