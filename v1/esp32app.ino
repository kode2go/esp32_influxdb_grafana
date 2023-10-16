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

EnergyMonitor emon1;
WiFiMulti WiFiMulti;
HTTPClient ask;
// TODO: user config
const char* ssid     = "xx"; //Wifi SSID
const char* password = "xx"; //Wifi Password

const char* serverUrl = "http://34.125.194.47/influxdb/write?db=esp02";
const char* code_version = "esp_32_flask_api_serial_01.5.py";
const String measurement = "data";

 
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
}


void loop() {

    double rawIrms = emon1.calcIrms(3000); // Calculate Irms only

    double power = rawIrms * 230.0;
    String dataToSend = measurement + " value=" + String(power);    
    Serial.print("Power: ");
    Serial.println(power);

    // Send POST request to Flask server
    HTTPClient http;
    http.begin(serverUrl);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    //int httpResponseCode = http.POST("data=" + dataToSend);
    int httpResponseCode = http.POST(dataToSend);

    if (httpResponseCode > 0) {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
    } else {
        Serial.print("Error sending POST request!");
        Serial.println(httpResponseCode);
    }

    http.end();

    delay(1000); // Send data every 1 seconds
}
