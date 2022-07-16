/*
 * ESP8266 CJMCU
 * Setup Programm to test sensor 
 * or establish connection to influxdb
 */
#include "Adafruit_CCS811.h"
#include <InfluxDbClient.h>
#include <ESP8266WiFiMulti.h>
 
#define LOOPDELAY 200
#define LED_PIN    2    

Adafruit_CCS811 ccs;

InfluxDBClient client;


// WiFi AP SSID
// WiFi password

#define DEVICE "Sensor_3"
ESP8266WiFiMulti wifiMulti;

// Data point



Point sensor("wifi_status");
Point ccs_sensor("CCS811");

void setup() {
  
  Serial.begin(9600);


  // Connect WiFi
  Serial.println("Connecting to WiFi");
  WiFi.mode(WIFI_STA);
  wifiMulti.addAP(WIFI_SSID, WIFI_PASSWORD);
  while (wifiMulti.run() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();

  // configure client
  client.setInsecure();
  client.setConnectionParamsV1(INFLUXDB_URL, INFLUXDB_DATABASE, INFLUXDB_USER, INFLUXDB_PASSWORD);

  // Add constant tags - only once
  sensor.addTag("device", DEVICE);
  ccs_sensor.addTag("device", DEVICE);
  
  sensor.addTag("SSID", WiFi.SSID());

   // Check server connection
  if (client.validateConnection()) {
    Serial.print("Connected to InfluxDB: ");
    Serial.println(client.getServerUrl());
  } else {
    Serial.print("InfluxDB connection failed: ");
    Serial.println(client.getLastErrorMessage());
  }



  
  //Serial.println("Testing the connection to CJMCU8118");  
  pinMode(LED_BUILTIN, OUTPUT);
  if(!ccs.begin()){
    Serial.println("Failed to start sensor! Please check your wiring.");
    ESP.restart();
  }

  //calibrate temperature sensor
  while(!ccs.available()){
    float temp = ccs.calculateTemperature();
    //ccs.setTempOffset(-temp);
  }
  Serial.print("CO2[ppm/1000]");
  Serial.print("\t");
  Serial.print("TVOC[ppb]");
  Serial.print("\t");
  Serial.println("T[°C]");
}

bool statusLED = false;

void loop() {  
  
  statusLED = !statusLED;
  digitalWrite(LED_BUILTIN, statusLED);   // Arduino: turn the LED on (HIGH)
  /*
  if(statusLED){
    Serial.println("LOOP LED ON");    
  }else{
    Serial.println("LOOP LED OFF");  
  }
  */
  if(ccs.available()){
    float temp = ccs.calculateTemperature();
    if(!ccs.readData()){
      Serial.print(ccs.geteCO2()/1000);
      Serial.print("\t");
      Serial.print(ccs.getTVOC());
      Serial.print("\t");
      Serial.println(temp-70);
    }else{
      Serial.println("ERROR!");
      ESP.restart();
    }
    ccs_sensor.clearFields();
    ccs_sensor.addField("CO2", ccs.geteCO2());
    ccs_sensor.addField("TVOCssure", ccs.getTVOC());
    ccs_sensor.addField("temp", temp);
  }

  
  

  // put your main code here, to run repeatedly:
  // Store measured value into point
  sensor.clearFields();

  // Report RSSI of currently connected network
  sensor.addField("rssi", WiFi.RSSI());
  // Print what are we exactly writing
  Serial.print("Writing: ");
  Serial.println(client.pointToLineProtocol(sensor));
   Serial.println(client.pointToLineProtocol(ccs_sensor));
  // If no Wifi signal, try to reconnect it
  if (wifiMulti.run() != WL_CONNECTED) {
    Serial.println("Wifi connection lost");
    ESP.restart();
  }
  // Write point
  if (!client.writePoint(sensor)) {
    Serial.print("InfluxDB write failed: ");
    Serial.println(client.getLastErrorMessage());
    ESP.restart();
  }
  // Write point
  if (!client.writePoint(ccs_sensor)) {
    Serial.print("InfluxDB write failed: ");
    Serial.println(client.getLastErrorMessage());
    ESP.restart();
  }

 
  //Wait 30s
  Serial.println("Wait 30s");
  
  delay(LOOPDELAY);
}
