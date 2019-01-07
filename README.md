# AdamNet-Drive-Emulator
AdamNet Drive Emulator (ADE)

This is a beta. Use at your own risk.

https://youtu.be/EGIlHEr0xxA

This is an Arduino Mega 2560 based SD card disk drive emulator. It supports 100 files and long file names (up to 40 characters are displayed).  The following materials are used:

 1. Arduino Mega 2560
 2. 1602 Keypad Shield - Arduino Version
 3. SD Card Shield - Micro SD with level converter
 4. 6 connector phone cable with 100ohm resistor.

How it is connected:

 Look at the schematic.jpg file.

 1. Mount the 1602 Keypad Shield to the Mega.
 2. Connect the SD Card Shield to the Mega.
 	Use the following pins: MISO = 50, MOSI = 51, SCK = 52, CS = 53, 5v and GND
 3. Attached the phone cord to pin 21 though the 100 ohm resistor. Connect the ground. 

How it works:

Place up to 100 files in the root of a FAT32 SD card. These need to be in the DSK format commonly found on the web. On startup of the Arduino there is no mounted file. Scroll though the files using the up and down keys. Unmount or mount the currently selected file with the right key. Unmount with the left key. The currently mounted file for the drive is shown on the top row. The select key will change the current drive.
