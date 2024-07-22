#FDC1004 

This Library provides basic functionality for the FDC1004 Capacitance to Digital Sensor. The FDC1004 provides femto-farad precision for measuring capacitances up to 115pF in 
incrementes of 500 aF.

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
 * The convenience functions in the .cpp file (written by Beshaya) set the measurement type (single or continuous, which channels, single-ended or differential)
 * They also trigger the measurement. For streaming/continuous measurement, this only needs to occur once. For single measurements, the cap-to-dig board must be triggered before each 
read.
 * MSB (16 bits) and LSB (16 bits) must be read from the cap-to-dig board before the next read. 
 * Because the relative capacitance measurement range is +/- 15 pF with a resolution of 500 aF, the least significant bit of a 16 bit value in two's complement represents the 
measurement 
resolution
 * 15 pF * (1 bit / 32 767 bits) = 460 aF
 * Therefore, this code ignores the LSB entirely from the capacitance calculation.   
