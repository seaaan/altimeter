// Code to test the time it takes to query each sensor and save the data
// Serial output saved to a CSV file as milliseconds per step 

#include <Wire.h>
extern "C"{
  #include "altitudesensor.h"
};
#include "RTClib.h"
#include <SD.h>

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
    Serial.println("RTC,Altitude,Temperature,X,Y,Z,SD");
    altimeter.startReadingAltitude();
}

void loop() {
  long start = millis();
  writeLine();
  Serial.println(millis() - start);
}

void writeLine() {
    long current = millis();
    String timeline = "";
    String readings;  
    
    while(!altimeter.isAltitudeReady()) {
      delay(10);
    }

    timeline = String(millis() - current) + ",";
    current = millis();

    float altitude = altimeter.readAltitude(); 
    // get sensor going on the next reading while we
    // run the rest of the functions
    altimeter.startReadingAltitude();

    timeline = timeline + String(millis() - current) + ",";
    current = millis();
  
    DateTime now = RTC.now();
    readings = String(now.hour()) + ':' +
        String(now.minute()) + ':' +
        String(now.second()) + ","; 

    timeline = timeline + String(millis() - current) + ",";
    current = millis();
            
    timeline = timeline + String(millis() - current) + ",";
    current = millis();   

    readings = readings + String(floatToString(altitude)) + ",";

    timeline = timeline + String(millis() - current) + ",";
    current = millis();   

    readings = readings + String(floatToString(altimeter.getTemperature())) + ",";

    timeline = timeline + String(millis() - current) + ",";
    current = millis();   
    
    readings = readings + String(analogRead(x)) + ",";

    timeline = timeline + String(millis() - current) + ",";
    current = millis();   

    readings = readings + String(analogRead(y)) + ",";

    timeline = timeline + String(millis() - current) + ",";
    current = millis();   
    
    readings = readings + String(analogRead(z));

    timeline = timeline + String(millis() - current) + ",";
    current = millis();   

    saveToFile(readings);
   
    timeline = timeline + String(millis() - current);
    current = millis();
    Serial.println(timeline);
}

void saveToFile(String readings) {
    const int CYCLES_BETWEEN_SAVES = 10;

    TO_WRITE += readings;
    
    if (cyclesSinceSave < CYCLES_BETWEEN_SAVES) {
      cyclesSinceSave++;
      TO_WRITE += "\n";
    } else {
      cyclesSinceSave = 0; 
      Serial.println(TO_WRITE);
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
