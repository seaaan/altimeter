// wiring: 
// gnd to gnd
// 5V to 5V
// SDA to A4
// SCL to A5

#include <Wire.h>
#include <Adafruit_MPL3115A2.h>
#include "RTClib.h"

Adafruit_MPL3115A2 altimeter = Adafruit_MPL3115A2();
RTC_DS1307 RTC;
const int z = A0;
const int y = A1;
const int x = A2;

void setup() {
  Serial.begin(9600);
  Wire.begin();
  RTC.begin();
  if (!altimeter.begin()) {
    Serial.println("Couldn't find sensor");
    return;
  }
  
  pinMode(z, INPUT);
  pinMode(y, INPUT);
  pinMode(x, INPUT);
  Serial.println("Date,Pascals,Meters,Celsius,X,Y,Z");
}

void loop() {
    DateTime now = RTC.now();
    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(' ');
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.print(",");
  
  // get*() functions return floats
  Serial.print(altimeter.getPressure());
  Serial.print(",");
  Serial.print(altimeter.getAltitude()); 
  Serial.print(",");
  Serial.print(altimeter.getTemperature());
  Serial.print(","); 

 Serial.print(analogRead(x));
 Serial.print(","); 
 Serial.print(analogRead(y)); 
 Serial.print(","); 
 Serial.println(analogRead(z));

  delay(250);
}

