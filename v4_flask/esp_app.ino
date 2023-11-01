/*
  Connect ESP32 to Cloud
 * Description:  This sketch connects to a flask cloud sending power data
 *  Author: kode2go
 *  github: kode2go
 *  01.2:
 *  send device number
 *  
 *  01.1:
 *  serial port code version*  
 *  
 */
 
#include <WiFi.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>
#include "EmonLib.h"
#include <Arduino.h> // Include the Arduino.h library for ESP32
#include <driver/adc.h> 
#include <ArduinoJson.h>

EnergyMonitor emon1;
WiFiMulti WiFiMulti;
HTTPClient ask;
// TODO: user config
const char* ssid     = "x"; //Wifi SSID
const char* password = "x"; //Wifi Password

const char* serverUrl = "http://34.125.194.47/influxdb/query?db=esp02";
const char* query = "q=SELECT LAST(*) FROM switch";
const char* code_version = "esp_32_flask_api_serial_01.5.py";
const String measurement = "switch";
const char* device_num = "esp01";

// Define the pin number for D13
const int d15Pin = 15; // Pin D13

int state = 0;
 
void setup(){

  adc1_config_channel_atten(ADC1_CHANNEL_6, ADC_ATTEN_DB_11);
  analogReadResolution(10);
  emon1.current(34, 30);
  
  // open serial
  Serial.begin(115200);
  Serial.println("*****************************************************");
  Serial.println("Code version: ");
  Serial.println(code_version);
  Serial.println("Device Number: ");
  Serial.println(device_num);

  Serial.println("********** Program Start : Connect ESP32 to WiFi.");
  Serial.println("Wait for WiFi... ");

  // connecting to the WiFi network
  WiFiMulti.addAP(ssid, password);
  while (WiFiMulti.run() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  // connected
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // Initialize the D15 pin as an output
  pinMode(d15Pin, OUTPUT);
  // Set the initial state of D15 to LOW
  digitalWrite(d15Pin, LOW);
  pinMode(0, INPUT_PULLUP); // Use internal pull-up resistor for the button
}


void loop() {
  
    double rawIrms = emon1.calcIrms(3000); // Calculate Irms only

    double power = rawIrms * 230.0;

    // Check if power is greater than 1500, if state == 10 keep pin high, else low
    if (state == 10)
    {
      Serial.println("state == 10 - ON");
      if (power > 1500) {
          digitalWrite(d15Pin, LOW);
      } else {
          digitalWrite(d15Pin, HIGH);
      }
    }
    else
    {
      Serial.println("state == 0 - OFF");
      digitalWrite(d15Pin, LOW);
    }
    
    String dataToSend = measurement + " value=" + String(power);    
    Serial.print("Power: ");
    Serial.println(power);

    // Send POST request to Flask server
    HTTPClient http;
    http.begin(serverUrl);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    int httpResponseCode = http.POST(query);

  if (httpResponseCode > 0) {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
      DynamicJsonDocument doc(1024);
      deserializeJson(doc, http.getString());
      state = doc["results"][0]["series"][0]["values"][0][1];
      Serial.print("Last Value: ");
      Serial.println(state);
  } else {
      Serial.print("Error sending POST request!");
      Serial.println(httpResponseCode);
  }

    http.end();

    delay(1000); // Send data every 1 seconds
}
