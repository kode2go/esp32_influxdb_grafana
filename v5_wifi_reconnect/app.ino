#include <WiFi.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>
#include <EmonLib.h>
#include <Arduino.h>
#include <driver/adc.h>

EnergyMonitor emon1;
WiFiMulti WiFiMulti;
HTTPClient ask;
// TODO: user config
const char* ssid     = "xx"; // Wifi SSID
const char* password = "xx"; // Wifi Password

const char* serverUrl = "http://35.239.72.184/influxdb/write?db=esp02";
const char* code_version = "esp_32_flask_api_serial_01.5.py";
const String measurement = "data";
const char* device_num = "esp04";

// Define the pin number for D13
const int d15Pin = 15; // Pin D13

// Function to connect to WiFi
void connectToWiFi() {
  Serial.println("********** Connect ESP32 to WiFi.");
  Serial.println("Wait for WiFi... ");
  WiFiMulti.addAP(ssid, password);
  while (WiFiMulti.run() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void setup() {
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

  pinMode(d15Pin, OUTPUT);
  digitalWrite(d15Pin, LOW);
}

void loop() {
  // Attempt to connect to WiFi if not connected
  if (WiFiMulti.run() != WL_CONNECTED) {
    connectToWiFi();
  }

  double rawIrms = emon1.calcIrms(3000);
  double power = rawIrms * 230.0;

  if (power > 1500) {
    digitalWrite(d15Pin, LOW);
  } else {
    digitalWrite(d15Pin, HIGH);
  }

  String dataToSend = measurement + " value=" + String(power);
  Serial.print("Power: ");
  Serial.println(power);

  HTTPClient http;
  http.begin(serverUrl);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  int httpResponseCode = http.POST(dataToSend);

  if (httpResponseCode > 0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
  } else {
    Serial.print("Error sending POST request!");
    Serial.println(httpResponseCode);
  }

  http.end();

  delay(1000);
}
