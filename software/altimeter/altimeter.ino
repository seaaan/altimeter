// SDA to A4
// SCL to A5

// MOSI - pin 11
// MISO - pin 12
// CLK - pin 13
// CS - pin 10     
// CD - nothing
// 3v - nothing

#include <Wire.h>
#include "RTClib.h"
#include <SD.h>

extern "C"{
  #include "altitudesensor.h"
};

Adafruit_MPL3115A2 altimeter = Adafruit_MPL3115A2();
RTC_DS1307 RTC;

String TO_WRITE = "";
int cyclesSinceSave = 0;

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
    
    // need to start the sensor before entering the loop
    altimeter.startReadingAltitude();
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
    writeLine();
}

void writeLine() {
    while(!altimeter.isAltitudeReady()) {
      delay(50);
    }
    
    float altitude = altimeter.readAltitude();
  
    // get sensor going on the next reading while we
    // run the rest of the functions
    altimeter.startReadingAltitude();
  
    DateTime now = RTC.now();
    String readings = String(now.hour()) + ':' +
        String(now.minute()) + ':' +
        String(now.second()) + "," + 
        String(floatToString(altitude)) + "," + 
        String(floatToString(altimeter.getTemperature())) + "," +
        String(analogRead(x)) + "," +
        String(analogRead(y)) + "," +
        String(analogRead(z));

    saveToFile(readings);
}

void saveToFile(String readings) {
    const int CYCLES_BETWEEN_SAVES = 10;
    
    TO_WRITE += readings;
    
    if (cyclesSinceSave < CYCLES_BETWEEN_SAVES) {
      cyclesSinceSave++;
      TO_WRITE += "\n";
    } else {
      cyclesSinceSave = 0; 
    
      File file = SD.open(fileCharArray, FILE_WRITE);
      file.println(TO_WRITE);
      file.close();
      TO_WRITE = "";
    }
}

String floatToString(float x) {
  long beforeDecimal = x; 

  x = x * 100;
  
  int afterDecimal = ((long) x) % 100; 
  
  return String(beforeDecimal) + "." + String(afterDecimal); 
}
