#include "stDHT.h"
DHT sens(DHT22); // Указать датчик DHT11, DHT21, DHT22

void setup() 
{
  Serial.begin(57600);
  pinMode(2, INPUT);
  digitalWrite(2, HIGH);
  pinMode(3, INPUT);
  digitalWrite(3, HIGH);
}

void loop() 
{
  int t = sens.readTemperature(2); // чтение датчика на пине 2
  int h = sens.readHumidity(2);    // чтение датчика на пине 2
  delay(2000);
  
  int t2 = sens.readTemperature(3); // чтение датчика на пине 3
  int h2 = sens.readHumidity(3);    // чтение датчика на пине 3
  delay(2000);
  
  Serial.print("Hum: ");
  Serial.print(h);
  Serial.print(" %");
  Serial.print("Temp: ");
  Serial.print(t);
  Serial.println(" C ");
  Serial.print("Hum2: ");
  Serial.print(h2);
  Serial.print(" %");
  Serial.print("Temp2: ");
  Serial.print(t2);
  Serial.println(" C "); 
}

