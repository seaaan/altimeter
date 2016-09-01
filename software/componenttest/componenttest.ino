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

bool rtcOn = true;
bool altimeterOn = true;
bool sdOn = true;

void setup() {
    Serial.begin(9600);    
    Wire.begin();
    
    if (!RTC.begin()) {
      Serial.println("RTC startup failed");
      rtcOn = false;
    }
    
    if (!altimeter.begin()) {
        Serial.println("Altimeter startup failed");
        altimeterOn = false;
    }

    if (!SD.begin(chipSelect)) {
        Serial.println("SD startup failed");
        sdOn = false;
    } else {
      createFile();
    }
    
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
    String nameToUse = String(dateTime.month()) +
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
    file.println("Date,Meters,Celsius,X,Y,Z");
    file.close();
}

void loop() {
  testRtc(); 
  testSd();
  testAltimeter();
  testAccelerometer();
  delay(1000);  
}

void testRtc() {
    if (rtcOn) {
    DateTime now = RTC.now();
    String time = "RTC on, time = " +
      String(now.hour()) + ':' +
      String(now.minute()) + ':' +
      String(now.second());
    
    Serial.println(time);
  } else {
    if (!RTC.begin()) {
      Serial.println("RTC startup failed");
      rtcOn = false;
    } else {
      Serial.println("RTC startup succeeded");
      rtcOn = true;
    }
  }
}

void testSd() {
  if (sdOn) {  
    Serial.println("SD on");
  } else {
    if (!SD.begin()) {
      Serial.println("SD startup failed");
      sdOn = false;
    } else {
      Serial.println("SD startup succeeded");
      sdOn = true;
    }
  }
}

void testAltimeter() {
  if (altimeterOn) {
    String altimeterString =  "Altimeter on, altitude = " +
      String(floatToString(altimeter.getAltitude())) + ", temperature = " + 
      String(floatToString(altimeter.getTemperature()));    
    Serial.println(altimeterString);
  } else {
    if (!altimeter.begin()) {
      Serial.println("Altimeter startup failed");
      altimeterOn = false;
    } else {
      Serial.println("Altimeter startup succeeded");
      altimeterOn = true;
    }
  }
}

void testAccelerometer() {
  String accelerometerString = "Accelerometer reading x = " + String(analogRead(x)) + 
    ", y = " + String(analogRead(y)) + 
    ", z = " + String(analogRead(z));
  Serial.println(accelerometerString);
}

String floatToString(float x) {
  long beforeDecimal = x; 

  x = x * 100;
  
  int afterDecimal = ((long) x) % 100; 
  
  return String(beforeDecimal) + "." + String(afterDecimal); 
}
