#define DISABLE_ALL_LIBRARY_WARNINGS // Disables all library warnings

#include <Arduino.h>
#include <WiFi.h>
#include <ThingSpeak.h>

// define wifi access point here
#define WIFI_SSID "nakano 2.4G" // The name of the WiFi access point
#define WIFI_PASS "19850916" // The password of the WiFi access point
//#define WIFI_SSID "Aatusa" // The name of the WiFi access point
//#define WIFI_PASS "aatusa01"// The password of the WiFi access point
WiFiClient client; // The client that connects to the WiFi network

// define ThingSpeak keys here
#define Channel_ID 2115495 // The ID of the ThingSpeak channel to update
#define Write_API_Key "93IOCSUWIY7YBRIC" // The Write API key of the ThingSpeak channel
#define Read_API_Key "J5KTZH756C37JNPK" // The Read API key of the ThingSpeak channel

int pir = GPIO_NUM_36; // The pin number of the motion sensor
int led = GPIO_NUM_25; // The pin number of the LED
int mic_digital = GPIO_NUM_26; // The pin number of the digital output of the microphone
int mic_analog = GPIO_NUM_39; // The pin number of the analog output of the microphone
int motionState = LOW; // The current state of the motion sensor's pin
float a_val; // The analog value of the microphone's output
int mic_max_temp = 0; // The maximum value of the microphone's output during calibration
float calibrated_a_val; // The calibrated value of the microphone's output

void calibrate_microphone()
{
  const unsigned long calibration_time = 10000; // The time (in ms) to calibrate the microphone
  unsigned long start_time = millis(); // The start time of the calibration
  Serial.println(" ");
  Serial.print("Calibrating the microphone");
  while (millis() - start_time < calibration_time) // While the calibration time has not elapsed
  {
    Serial.print(".");
    int mic_val = analogRead(mic_analog); // Read the value of the microphone's output
    if (mic_val > mic_max_temp) // If the value is greater than the previous maximum
    {
      mic_max_temp = mic_val; // Update the maximum value
    }
    delay(10);
  }
  calibrated_a_val = ((float)mic_max_temp * 100) / 4095; // Calibrate the microphone's output
  // Print calibration results
  Serial.println(" ");
  Serial.print("max value: ");
  Serial.println(calibrated_a_val);
}

void httpRequest(int field1Data, float field2Data)
{
  // Initialize ThingSpeak
  ThingSpeak.begin(client);

  // Set the fields with the values
  ThingSpeak.setField(1, field1Data);
  ThingSpeak.setField(2, field2Data);

  int res = ThingSpeak.writeFields(Channel_ID, Write_API_Key); // Update the ThingSpeak channel with the data
  if (res == 200) // If the update is successful
  {
    Serial.println("Channel update success.");
  }
  else // If the update fails
  {
    Serial.print("Channel update error: ");
    Serial.println(res);
  }
  client.stop(); // Disconnect from the ThingSpeak server
}

void setup()
{
  Serial.begin(9600); // initialize serial communication

  pinMode(mic_digital, INPUT); // set KY-037 pin to input mode
  pinMode(pir, INPUT);         // set ESP32 pin to input mode
  pinMode(led, OUTPUT);        // set ESP32 pin to output mode

  // We start by connecting to a WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);

  WiFi.begin(WIFI_SSID, WIFI_PASS); // Connect to WiFi network using SSID and password

  while (WiFi.status() != WL_CONNECTED) // Wait for WiFi to connect
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP()); // Print local IP address

  digitalWrite(led, LOW); // Turn off LED

  calibrate_microphone(); // calibrate mic
}

void loop()
{
  int count = 0; // count number of detections
  double sensitivity_adjust = 0.6; // adjust sensitivity of microphone
  float max_a_val = -1; // initialize max_a_val to a small value

  // Run loop for 60 iterations (60 seconds)
  for (int i = 0; i < 60; i++)
  {
    a_val = (float)analogRead(mic_analog); // read analog input from microphone
    a_val = ((float)a_val * 100) / 4095; // convert analog value to percentage
    Serial.print("Current noise: ");
    Serial.print(a_val);
    Serial.print(" | ");
    Serial.print("Max noise: ");
    Serial.print(calibrated_a_val - sensitivity_adjust); // print current and max noise levels
    Serial.print(" | ");
    Serial.print("Motion state: ");
    Serial.println(motionState);

    if (a_val > max_a_val) // update max_a_val if a_val is greater
    {
      max_a_val = a_val;
    }

    motionState = digitalRead(pir); // read motion sensor state

    if ((motionState) && (a_val > calibrated_a_val - sensitivity_adjust)) // if motion detected AND noise level is higher than calibrated value
    {              
      count++; // increment detection count
      Serial.println("***************************************");
      Serial.print("volume: ");
      Serial.println(a_val);
      Serial.print("count: ");
      Serial.println(count); // print volume and count
      Serial.println("***************************************");
      digitalWrite(led, HIGH); // turn on LED
      delay(1000); // delay for 1 second
      digitalWrite(led, LOW); // turn off LED
      max_a_val = a_val; // update max_a_val
    }
    delay(1000); // delay for 1 second
  }

  if (count > 0) // if detections were made
  {
    Serial.println("Intruder!!");
    httpRequest(count, max_a_val); // send HTTP request with count and max_a_val
  }
  else // if no detections were made
  {
    Serial.println("No Intruder so far");
    httpRequest(0, max_a_val); // send HTTP request with count=0 and max_a_val
  }
}
