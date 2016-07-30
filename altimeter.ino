// SDA to A4
// SCL to A5

// MOSI - pin 11
// MISO - pin 12
// CLK - pin 13
// CS - pin 10     
// CD - nothing
// 3v - nothing

#include <Wire.h>
#include <Adafruit_MPL3115A2.h>
#include "RTClib.h"
#include <SD.h>

Adafruit_MPL3115A2 altimeter = Adafruit_MPL3115A2();
RTC_DS1307 RTC;

const int chipSelect = 10;
const int z = A0;
const int y = A1;
const int x = A2;

char* fileCharArray;

void setup() {
    Serial.begin(9600);
    Wire.begin();
    RTC.begin();
    
    if (!altimeter.begin()) {
        Serial.println("Couldn't find altimeter");
        return;
    }

    if (!SD.begin(chipSelect)) {
        Serial.println("SD card failed or not present");
        return;
    }

    createFile();

    pinMode(z, INPUT);
    pinMode(y, INPUT);
    pinMode(x, INPUT);
}

// creates a file named with the date plus a numeric extension 
// stores that in the file global variable
void createFile() {
    DateTime dateTime = RTC.now();
    
    int ARRAY_SIZE = 20;
    
    fileCharArray = new char[ARRAY_SIZE];
    String nameToUse = String(dateTime.year()) + 
    String(dateTime.month()) +
    String(dateTime.day());

    nameToUse.toCharArray(fileCharArray, ARRAY_SIZE, 0);
    String tempName = nameToUse;
    int i = 0;
    while (SD.exists(fileCharArray)) {
        i++;
        tempName = nameToUse + String(i);
        tempName.toCharArray(fileCharArray, ARRAY_SIZE, 0);
    }

    File file = SD.open(fileCharArray, FILE_WRITE);
    file.println("Date,Pascals,Meters,Celsius,X,Y,Z");
    file.close();
}

void loop() {
    DateTime now = RTC.now();
    String readings = String(now.hour()) + ':' +
        String(now.minute()) + ':' +
        String(now.second()) + "," + 
        String(floatToString(altimeter.getPressure())) + "," +
        String((long) altimeter.getAltitude()) + "," + 
        String(floatToString(altimeter.getTemperature())) + "," +
        String(analogRead(x)) + "," +
        String(analogRead(y)) + "," +
        String(analogRead(z));

    File file = SD.open(fileCharArray, FILE_WRITE);
    file.println(readings);
    file.close();
    
    Serial.println(readings);
    altimeter.getAltitude();
    delay(1000);
}

String floatToString(float x) {
  long beforeDecimal = x; 

  x = x * 100;
  
  int afterDecimal = ((long) x) % 100; 
  
  return String(beforeDecimal) + "." + String(afterDecimal); 
}
