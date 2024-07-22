#FDC1004 

This Library provides basic functionality for the FDC1004 Capacitance to Digital Sensor. The FDC1004 provides femto-farad precision for measuring capacitances up to 115 pF in 
increments of 500 aF.

## Requirements

 * 3.3 V Microcontroller board (tested on ESP32 Feather V2 from Adafruit). This code will not work with 5 V microcontrollers!
 * FDC1004
 * 2 10k Ohm resistors to pull up SDA and SCL pins on I2C line (some microcontrollers may already have these on board)
 
## Limitations
 * Differential capacitive measurements have not been tested

## Capdac behavior
 * Capdac is an offset capacitance from 0 pF to 96 pF. It can be written to the cap-to-digital board only in single-ended mode, with a max value of 31 (5 bits)
 * The adjust capdac function increases capacitance by 15.6 pF, which is just outside the upper or lower limits of the range.
 * It then sets capdac based on the sigma-delta reading to get as close to 0 pF relative capacitance reading as possible.
 * If a capacitance value is not read, or it is out of the +/- 15 pF bounds, capdac resets to 0 and re-adjusts to find the new capdac.

## Single-ended capacitance (pin to ground) or differential (pin to pin) readings: 
 * If first pin and second pin in configureMeasurement and triggerMeasurement functions are the same, it assumes a sigle-ended measurement
 * Otherwise, it assumes a differential measurement between those pins.
 * The convenience functions in the .cpp file (written by Beshaya) set the measurement type (single or continuous, which channels, measurement number/register)
 * They also trigger the measurement. For streaming/continuous measurement, this only needs to occur once. For single measurements, the cap-to-dig board must be triggered before each 
read.
 * The absolute capacitance is a function of the capdac and the read MSB (value)
 * The measurement resolution is 0.5 fF and the max range is 30 pF 
 * A 16 bit two's complement number ranges from -32768 to 32767 (2^15 - 1) (i.e., resolution of ~0.46 fF with +/- 15 pF range)
 * Therefore, we don't need to calculate the capacitance using the LSB since the most significant bit of LSB is below the measurement resolution/noise floor
 * We do still need to read LSB since the FDC1004 is expecting it to be read before the next measurement. The convenience functions in the .cpp file take care of this read.
 * MSB (16 bits) and LSB (16 bits) must be read from the cap-to-dig board before the next read.    
