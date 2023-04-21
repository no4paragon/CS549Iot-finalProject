#define DISABLE_ALL_LIBRARY_WARNINGS
#include <Arduino.h>
#include <WiFi.h>
#include <ThingSpeak.h>
#include <HTTPClient.h>

// define wifi access point here
#define WIFI_SSID "nakano 2.4G"
#define WIFI_PASS "19850916"
WiFiClient client;

// define ThingSpeak keys here
#define Channel_ID 2115495
#define Write_API_Key "93IOCSUWIY7YBRIC"
#define Read_API_Key "J5KTZH756C37JNPK"

int pir = GPIO_NUM_36;
int led = GPIO_NUM_25;
int mic_digital = GPIO_NUM_26;
int mic_analog = GPIO_NUM_39;
int motionState = LOW; // current  state of motion sensor's pin
float a_val;           // analog value of mic
int mic_max_temp = 0;
float max_value;

void calibrate_microphone()
{
  // Read microphone values for 10 seconds. We need just a max value to identify unusual noise.
  const unsigned long calibration_time = 10000;
  unsigned long start_time = millis();
  Serial.println(" ");
  Serial.print("Calibrating the microphone");
  while (millis() - start_time < calibration_time)
  {
    Serial.print(".");
    int mic_val = analogRead(mic_analog);
    if (mic_val > mic_max_temp)
    {
      mic_max_temp = mic_val;
    }
    delay(10);
  }
  max_value = ((float)mic_max_temp * 100) / 4095;
  // Print calibration results
  Serial.println(" ");
  Serial.print("max value: ");
  Serial.println(max_value);
}

void httpRequest(int field1Data, float field2Data)
{
  // Initialize ThingSpeak
  ThingSpeak.begin(client);

  // Set the fields with the values
  ThingSpeak.setField(1, field1Data);
  ThingSpeak.setField(2, field2Data);

  int res = ThingSpeak.writeFields(Channel_ID, Write_API_Key);
  if (res == 200)
  {
    Serial.println("Channel update success.");
  }
  else
  {
    Serial.print("Channel update error: ");
    Serial.println(res);
  }
  client.stop();
}

void setup()
{
  Serial.begin(9600); // initialize serial

  pinMode(mic_digital, INPUT); // set KY-037 pin to input mode
  pinMode(pir, INPUT);         // set ESP32 pin to input mode
  pinMode(led, OUTPUT);        // set ESP32 pin to output mode

  calibrate_microphone(); // calibrate mic

  // We start by connecting to a WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);

  WiFi.begin(WIFI_SSID, WIFI_PASS);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  digitalWrite(led, LOW);
}

void loop()
{

  int count = 0; // counts numbers of detection
  float temp = 0;
  double sensitivity_adjust = 0.5;

  for (int i = 0; i < 60; i++)
  {
    a_val = (float)analogRead(mic_analog);
    a_val = ((float)a_val * 100) / 4095;
    Serial.print("Current noise: ");
    Serial.print(a_val);
    Serial.print(" | ");
    Serial.print("Max noise: ");
    Serial.println(max_value - sensitivity_adjust);

    motionState = digitalRead(pir); // read new state

    if ((motionState) && (a_val > max_value - sensitivity_adjust)) // when motion found AND exceeds max noise.
    {
      Serial.print("volume: ");
      Serial.println(a_val);
      count++;
      Serial.print("count: ");
      Serial.println(count);
      digitalWrite(led, HIGH);
      delay(1000); // delay a second
      digitalWrite(led, LOW);
    }
    else
    {
      if (a_val > temp) // when motion is not detected, we keep the highest value for noise.
      {
        temp = a_val;
      }
      delay(1000); // delay a second
    }
  }

  if (count > 0)
  {
    httpRequest(count, a_val);
  }
  else
  {
    httpRequest(0, temp);
    Serial.print("No Intruder so far");
  }
}
