# FDC1004 Single-ended and Differential Read Examples

This Library provides basic functionality for the FDC1004 Capacitance to Digital Sensor. The FDC1004 provides femto-farad precision for measuring capacitances up to 115 pF in increments of 500 aF. The .ino files in examples demonstrate reading continuous values, triggered single values for multiple channels, and differential measurements. 

## Hardware

 * A 3.3 V microcontroller board. The FDC1004 is incompatible with 5 V microcontrollers without additional care for the I2C bus voltage!
 * FDC1004
 * 2 10k Ohm resistors to pull up SDA and SCL pins on I2C line (some microcontrollers may already have these on board)
 * A few non-polarized (e.g., ceramic) capacitors for test, in the range of 5 pF to 100 pF
 
## Limitations
 * This code has only been tested on the Adafruit ESP32 Feather V2 microcontroller. It may not work on other 3.3 V microcontrollers. 

## Single-ended capacitance (channel to ground) or differential (channel to channel) readings: 
 * If first pin and second pin in configureMeasurement and triggerMeasurement functions are the same, it assumes a sigle-ended measurement
 * Otherwise, it assumes a differential measurement between those pins.
 * The convenience functions in the .cpp file (written by Beshaya) set the measurement type (single or continuous, which channels, measurement number/register)
 * Capacitors must always be placed from channel to ground. In differential measurements, they are still read by the difference between capacitance channels to ground. 

 ### Documentation on example functions
 
### float absoluteCapacitance(uint8_t measSlot, uint8_t chanA, uint8_t chanB, uint8_t measType, uint8_t * capdacPtr)
 * Calculates the absolute capaciance capacitanceAbsolute (relative + capdac) read off the FDC1004

 #### Parameters: 
 * measSlot : the measurement "slot" or number. This can be arbitrary between 0 and 3, or measA, measB... for convenience
 * chanA : the first pin you wish to measure capacitance to ground. Use pin definitions above for convenience.
 * chanB : the second pin you wish to measure capacitance to ground. Due to limitations of the FDC1004, chanB must be the same or greater value to chanA. Setting to the same value as chanA will set up a single-ended measurement. Setting at a higher value will set up a differential measurement. 
 * measType : Single (0) or repeated (1). Use measSingl and measCont for convenience. 
 * capdacPtr : A pointer to the global capdac value for that measurement. Labeled capdacA, capdacB... for convenience

 #### returns capacitanceAbsolute

* Notes: Calls configTrigRead in the case of a single measurement. Calls relativeCapacitance value in the case of a repeated measurement. If the returned capacitanceRelative reading is out of the +/- 15 pF limits for the FDC1004, it will call setCAPDAC. It converts capdac to a capacitance value in pF and adds to relative capacitance to find capacitanceAbsolute. If setCAPDAC was called, it will set the value of to capacitanceAbsolute -1

### float relativeCapacitance(uint8_t measSlot)
  * Reads the relative capacitance capacitanceRelative

#### Parameters: 
  ** measSlot : the measurement "slot" or number. This can be arbitrary between 0 and 3, or measA, measB... for convenience

* returns capacitanceRelative

* Notes: calls fdc.readMeasurement to read a 32-bit value (24-bit capacitance with 8 trailing zeroes, as per FDC1004 documentation) and convert into a capacitance reading with the ratio 15 pF/ 32676. The 16 most significant bits (MSB) are the only ones needed for the capacitance reading, since the measurement resolution is 0.5 fF and the max range is 30 pF. A 16 bit value in two's complement ranges from -32768 to 32767 (2^15 - 1) (i.e., resolution of ~0.46 fF with +/- 15 pF range). Therefore, we don't need to calculate the capacitance using the LSB since the most significant bit of LSB is below the measurement resolution/noise floor. We do still need to read LSB since the FDC1004 is expecting it to be read before the next measurement. The convenience functions in the .cpp file take care of this read.

### uint8_t setCAPDAC(uint8_t measSlot, uint8_t chanA, uint8_t chanB, uint8_t measType, uint8_t * capdacPtr)
* Resets capdac value. Will only execute if the measurement is single-ended (chanA and chanB are the same), because capdac cannot be set for a differential measurement.

#### Parameters:  
* measSlot : the measurement "slot" or number. This can be arbitrary between 0 and 3, or measA, measB... for convenience
* chanA : the first pin you wish to measure capacitance to ground. Use pin definitions above for convenience.
* chanB : the second pin you wish to measure capacitance to ground. Due to limitations of the FDC1004, chanB must be the same or greater value to chanA. Setting to the same value as chanA will set up a single-ended measurement. Setting at a higher value will set up a differential measurement. 
* measType : Single (0) or repeated (1). Use measSingl and measCont for convenience. 
* capdacPtr : the pointer to the global capdac associated with the measurement in measSlot

* returns *capdacPtr (the dereference to the pointer to the global capdac associated with that measurement)

* Notes: Resets capdac to 0 and calls configTrigRead to read the relative value. While the new capdac value has not been found, it calls adjustCAPDAC, then calls configTrigRead in a loop until the timeout value (set to 30 by default). If a new capdac value is not found, it defaults to 0.


### uint8_t adjustCAPDAC(float capacitanceRelative, uint8_t * capdacPtr)
* Adjusts or increments capdac value and returns a flag stating whether it successfully reset.
 
#### Parameters: 
* capacitanceRelative : the relative capacitance reading fom the FDC
* capdacPtr : the pointer to the global capdac associated with the measurement in measSlot

* returns capdacFlag, which says whether or not the capdac has been successfully reset.

* Notes: If the capdac is in range, it will try to find the capdac value that brings capacitanceRelative as close to 0 as possible to give the largest potential range, then return 0 to signal that capdac has been reset successfully. If capdac is not in range, it will incremenet or decrement capdac by 5 (15.625 pF) to bring it to the next range, then return 1 to signal that capdac needs to be set again.

### float configTrigRead(uint8_t measSlot, uint8_t chanA, uint8_t chanB, uint8_t measType, uint8_t capdac)
 * Configures and triggers the measurement and reads relative capacitance

 #### Parameters: 
 * measSlot : the measurement "slot" or number. This can be arbitrary between 0 and 3, or measA, measB... for convenience
 * chanA : the first pin you wish to measure capacitance to ground. Use pin definitions above for convenience.
 * chanB : the second pin you wish to measure capacitance to ground. Due to limitations of the FDC1004, chanB must be the same or greater value to chanA. Setting to the same value as chanA will set up a single-ended measurement. Setting at a higher value will set up a differential measurement. 
 * measType : Single (0) or repeated (1). Use measSingl and measCont for convenience. 
 * capdac : the global capdac value associated with that measurement.

 * returns capacitanceRelative 
  
