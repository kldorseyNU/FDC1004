# Breakout board for the FDC1004
To work with the FDC1004 in portable applications, we have created a breakout board with an ESP32-S3 Xiao footprint, the FDC1004 capacitive sensor, and an active thermistor for temperature compensation. 

## Ordering the board: 
The board files are available in the Github repo. The footprint for all components is “most” (lowest component density/easiest for hand soldering), and you may also wish to order solder paste masks. The boards were tested with Osh Park boards: https://oshpark.com/?ref=grounded and are shared on the OshPark website which you can find by searching k.dorsey@northeastern.edu 

## Soldering the board: 
1. Solder R1, R2, U1, U3, and J1-4 to the front side of the board using whichever surface mount technique you are most comfortable with. It is challenging but possible to solder by hand all with a fine tip because the footprints are all “most” (lowest chip density). If you are soldering by hand, populate in this order: U3, U1, R1, R2, J1-4
2. Do not populate J5 yet.
3. Solder five wires to the back side on pins GND, 3V, SDA, SCL, and A0. 
4. Do a connectivity test with a multimeter to make sure there is no short between the 3V and GND pins. You should measure a resistance of about 2 MOhms.
5. Write the test sketch to the ESP32-S3 microcontroller. You will need to hit the reset button after uploading to get it to begin running.
6. Connect the board to the microcontroller via the soldered wires on the backside and reset the microcontroller.
7. Replace/rework the MCP9701 or FDC1004 chips as necessary if either test failed. 
8. De-solder the test wires.
9. If desired, populate J5 for external connections to GND or to operate with a battery rather than USB-C.
10. Solder the microcontroller to the backside of the board using the castellated pins. 
11. Do a connectivity test with a multimeter to make sure there is no short between the 3V and GND pins. You should measure a resistance of about 2.8 kOhms.
12. Re-test.
13. Write the capacitance measurement sketch you desire to the microcontroller. Congrats! You're done.

