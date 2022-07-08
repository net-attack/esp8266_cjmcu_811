/*
 * ESP8266 CJMCU
 * Setup Programm to test sensor 
 * or establish connection to influxdb
 */
#include "Adafruit_CCS811.h"

 
#define LOOPDELAY 200
#define LED_PIN    2    

Adafruit_CCS811 ccs;


void setup() {
  
  Serial.begin(9600);
  //Serial.println("Testing the connection to CJMCU8118");  
  pinMode(LED_BUILTIN, OUTPUT);
  if(!ccs.begin()){
    Serial.println("Failed to start sensor! Please check your wiring.");
    while(1);
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
      while(1);
    }
  }
  delay(LOOPDELAY);
}
