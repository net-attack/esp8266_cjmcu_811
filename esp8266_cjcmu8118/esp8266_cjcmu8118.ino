/*
 * ESP8266 CJMCU
 * Setup Programm to test sensor 
 * or establish connection to influxdb
 */
#define LOOPDELAY 100

void setup() {
  Serial.begin(9600);
  Serial.println("Testing the connection to CJMCU8118")
}

int x = 0;
void loop() {
  Serial.println(x++);
  delay(LOOPDELAY);
}
