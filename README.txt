After extracting the KISS_OSD folder copy the MAX7456 folder to you Arduino IDE "libraries" folder before you start to compile.

Bevor Ihr mit der Kompilierung mit der Arduino IDE beginnen könnt, müsst Ihr den enthaltenen MAX7456 Ordner in der "libraries" Ordner der Arduino IDE kopieren.


How to flash your OSD:
1) Download MW-OSD, KISS OSD and Arduino 1.0.5
2) Connect FTDI-Adapter to your OSD and to your PC
3)Install MWOSD. See this: https://quadmeup.com/how-to-flash-mw-osd-to-micro-minimosd/
4)Open the MWOSD-GUI and connect to the OSD
5)In the section "font-tools" click on "select" and select one of the fonts included in the Kiss-OSD release
6)Still in "font-tools" click on "Upload". Wait until completed.
7)Click "Disconnect" and close the GUI
8)Copy the "MAX7456" folder in this release in the "libraries" folder of the Arduino-Installation (C:\Program Files (x86)\Arduino\libraries")
9)Open the Arduino-Programm. Go to "Files", "Examples", "EEPROM" and then "EEPROM_clear"
10)Compile and run it
11)Open the KISS-OSD.ino from this folder and make the changes you need in the beginning of the file
12)Compile and flash it.
13)Enjoy :)
