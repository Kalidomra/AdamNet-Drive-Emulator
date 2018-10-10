# ADAMNet-Drive-Emulator
ADAMNet Drive Emulator (ADE)

This is beta. Use at your own risk.

This is an Arduino Mega 2560 based SD card disk drive emulator. It supports 100 files and long file names (up to 40 characters are displayed).  The following materials are used:

1 - Arduino Mega 2560
2 - 1602 Keypad Shield
3 - SD Card Shield
4 - 6 connector phone cable with 1k resistor.

How it is connected:

 1. Mount the 1602 Keypad Shield to the Mega.
 2. Connect the SD Card Shield to the Mega.
 	Use the following pins: MISO = 50, MOSI = 51, SCK = 52, CS = 53, 5v and GND
 3. Attached the phone cord to pin 21 though the 1k resistor and ground. 

How it works:

Place up to 100 files in the root of a FAT32 SD card. These need to be in the DSK format commonly found on the web. On startup of the Arduino there is no mounted file. Scroll though the files using the up and down keys. Mount the currently selected file with the right key. Unmount with the left key. The currently mounted file is shown on the top row. The select key will change the current device number and save to eeprom. You will need to reset the Adam on device number change.
