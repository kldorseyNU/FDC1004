/***********************************************************************
 FDC1004 Library

 This library provides functions for using TI's FDC1004 Capacitance to Digital Sensor
 
 Written by Benjamin Shaya

************************************************************************/

#include <FDC1004.h>

uint8_t MEAS_CONFIG[] = {0x08, 0x09, 0x0A, 0x0B};
uint8_t MEAS_MSB[] = {0x00, 0x02, 0x04, 0x06};
uint8_t MEAS_LSB[] = {0x01, 0x03, 0x05, 0x07};
uint8_t SAMPLE_DELAY[] = {11,11,6,3};

FDC1004::FDC1004(uint16_t rate){
  this->_addr = 0b1010000; //not configurable, to my knowledge
  this->_rate = rate;
}

void FDC1004::write16(uint8_t reg, uint16_t data) {
  Wire.beginTransmission(_addr);
  Wire.write(reg); //send address
  Wire.write( (uint8_t) (data >> 8));
  Wire.write( (uint8_t) data);
  Wire.endTransmission();
}

uint16_t FDC1004::read16(uint8_t reg) {
  Wire.beginTransmission(_addr);
  Wire.write(reg);
  Wire.endTransmission();
  uint16_t value;
  //Wire.beginTransmission(_addr);
  Wire.requestFrom(_addr, (uint8_t)2);
  value = Wire.read();
  value <<= 8;
  value |= Wire.read();
  //Wire.endTransmission();
  return value;
}

//configure a measurement (call only when changing the setup of a measurement)
uint8_t FDC1004::configureMeasurement(uint8_t measurement, uint8_t channel, uint8_t diffChannel, uint8_t capdac) {
    //Verify data
    if (!FDC1004_IS_MEAS(measurement) || !FDC1004_IS_CHANNEL(channel) || capdac > FDC1004_CAPDAC_MAX) {
        Serial.println("Measurement, channel, or capdac out of bounds");
        return 1;
    }

    //build 16 bit measurement configuration
    uint16_t configuration_data;
    configuration_data = ((uint16_t)channel) << 13; //CHA diff

    if (channel == diffChannel && capdac != 0){
        //set CAPDAC, disable differential excitation on EXB
        configuration_data |=  ((uint16_t)0x04) << 10; //CHB disable and CAPDAC enable
        configuration_data |= ((uint16_t) capdac) << 5; //CAPDAC value
    }
    else if (channel == diffChannel && capdac == 0){
        //Disable CAPDAC and differential excitation on EXB
        configuration_data |=  ((uint16_t)0x05) << 10; //CHB disable and CAPDAC disable
    }
    else {
        //disable CAPDAC, set differential excitation
        configuration_data |=  ((uint16_t)diffChannel) << 10; //CHB enable and CAPDAC disable
    }
    write16(MEAS_CONFIG[measurement], configuration_data);
    return 0;
}

uint8_t FDC1004::triggerMeasurement(uint8_t measurement, uint8_t rate, uint8_t measType) {
  //verify data
    if (!FDC1004_IS_MEAS(measurement) || !FDC1004_IS_RATE(rate)) {
       Serial.println("bad trigger request");
       return 1;
    }

    uint16_t trigger_data;
    trigger_data = ((uint16_t)rate) << 10; // sample rate
    trigger_data |= measType << 8; 
    trigger_data |= (1 << (7-measurement)); // 0 > bit 7, 1 > bit 6, etc
    write16(FDC_REGISTER, trigger_data);
    return 0;
}

uint8_t FDC1004::readMeasurement(uint8_t measurement, uint16_t * value) {
    if (!FDC1004_IS_MEAS(measurement)) {
        Serial.println("bad read request");
        return 1;
    }
    
    //check if measurement is complete
    uint16_t fdcRegister = read16(FDC_REGISTER);
    uint8_t delayCount = 0;
    uint8_t maxDelayCount = 10;
    while ((! (fdcRegister & ( 1 << (3-measurement)))) && delayCount < maxDelayCount) {
      //confirm with FDC that measurement is complete
      delay(10);
      delayCount++;
      fdcRegister = read16(FDC_REGISTER);
    }

    if (delayCount < maxDelayCount){
      //read the value
      uint16_t msb = read16(MEAS_MSB[measurement]);
      uint16_t lsb = read16(MEAS_LSB[measurement]);  
      value[0] = msb;
      value[1] = lsb;
      return 0;
    }
    else{
      //read failed or timed out
      return 1;    
    }  
}

//reset software of FDC1004
uint16_t FDC1004::resetDevice() {
  uint16_t resetData = 1 << 15; //reset is 0x1000
  uint8_t delayCount = 0;
  uint8_t maxDelayCount = 10;

  write16(FDC_REGISTER, resetData);
  delay(100);
  uint16_t fdcRegister = read16(FDC_REGISTER);
  return fdcRegister; //will return zero if properly reset
}

