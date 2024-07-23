/**************************************************************
 * Example Program for FDC1004 Library
 * This program will demonstrates reading the capacitance on CAP1 of the FDC1004
 * and changing capdac to try to auto-range the chip
 * Note that the output is a signed integer, so values greater than 7FFF FFFF are actually negative!
 **************************************************************
 * Setup
 * Connect 3.3V and Ground to the FDC1004 (don't use 5V, you'll fry your chip!)
 * Connect SDA and SCL to your Arduino
 * Power on and run this code
 * Open a Serial monitor at 115200 baud
 **************************************************************
 * Written by Benjamin Shaya for Rest Devices
 * bshaya@alum.mit.edu
 * https://github.com/beshaya/FDC1004
 **************************************************************/
 
#include <Wire.h>
#include <FDC1004.h>

uint8_t capdac = 0;
uint16_t measType = 1; // 0 is single read, 1 is continuous read
uint8_t measurement = 0; //Select measurement slot 0, 1, 2, or 3
int readRate = 100;
const float capMax = 14.9;
const float capdacConversion = 3.125;

FDC1004 fdc;

void setup() {
  Serial.begin(115200);
  delay(1000);
  Wire.begin();
  delay(1000);
  setCAPDAC();
}

void loop() {  
  delay(2500);
  absoluteCapacitance();
}

float absoluteCapacitance() {
  if (measType == 0){
      configTrigRead();
    }
  float capacitanceRelative = relativeCapacitance();
  if ((capacitanceRelative <= capMax) && (capacitanceRelative >= -capMax)){
    float capacitanceAbsolute = capacitanceRelative + (3.125 * (float)capdac); //converts capdac to pF
    Serial.println(capacitanceAbsolute);
    return capacitanceAbsolute;
  }
  else {
    Serial.println("Capacitance out of bounds, re-running setCAPDAC");
    setCAPDAC();
    return -1;
  }
}

float relativeCapacitance() {  
  delay(readRate);
  uint16_t value[2];
  float capacitanceRelative = 17;
  if (!fdc.readMeasurement(measurement, value)) {
    // The absolute capacitance is a function of the capdac and the read MSB (value[0])
    // The measurement resolution is 0.5 fF and the max range is 30 pF 
    // A 16 bit two's complement binary number ranges from -32768 to 32767 (i.e., resolution of ~0.46 fF with +/- 15 pF range)
    // Therefore, we don't need to calculate the capacitance using the LSB since the most significant bit of LSB is below the measurement resolution
    // We do still need to read LSB since the FDC1004 is expecting it to be read before the next measurement. The convenience functions in the .cpp file take care of this read.
    int16_t capBits = value[0];
    float capRatio = (15.0) / (32767.0); //This converts bits (2^15 - 1) into capacitance (+/- 15 pF range)
    capacitanceRelative = (float)(capBits) * capRatio; // value in picofarads (pF)
  }
  return capacitanceRelative;
}

int setCAPDAC(){
  capdac = 0;
  float capacitanceRelative = configTrigRead();
  uint8_t capdacTimeout = 0;

  while (capdacTimeout < 30){
    if (adjustCAPDAC(capacitanceRelative)){
      capdacTimeout = 31;
      capacitanceRelative = configTrigRead();
      return 1;
    }
    else{
      capdacTimeout++;
      capacitanceRelative = configTrigRead();
    }
  }
  return 0;
}

int adjustCAPDAC(float capacitanceRelative) {  
    if ((capacitanceRelative < capMax) && (capacitanceRelative > -1*capMax)){
      // if it's in range, adjust capdac so capacitance is as close to 0 as possible
      capdac += capacitanceRelative/capdacConversion;
      capdac = max(capdac, ((uint8_t)0));
      capdac = min(capdac, ((uint8_t)FDC1004_CAPDAC_MAX));
      return 1;
    }
    else if (capacitanceRelative <= -1*capMax) {
      capdac -= 5; //decrease by 15.625 pF, which is just outside the bounds of +/- 15 pF measurement range
      capdac = max(capdac, ((uint8_t)0));
      return 0;
    }
    else {
      capdac += 5; //increase by 15.625 pF, which is just outside the bounds of +/- 15 pF measurement range
      capdac = min(capdac, ((uint8_t)FDC1004_CAPDAC_MAX));
      return 0;
    } 
}

float configTrigRead(){
  fdc.configureMeasurement(measurement, FDC1004_Chan0, FDC1004_Chan0, capdac);
  fdc.triggerMeasurement(measurement, FDC1004_100HZ, measType);
  return relativeCapacitance();
}