#include <LiquidCrystal.h>
#include <util/delay.h>
#include <SPI.h>
#include <EEPROM.h>
#include "SdFat.h"
//============================================================================================================================================
//==================================             AdamNet Drive Emulator (ADE)        =========================================================
//============================================================================================================================================
const int NumberofDrives = 4;              // Number of Drives to Emulate. Max is 4.
const int AdamNetPin = 21;                 // AdamNet serial communication pin. (21 = INT0 = PD0)
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);       // Pins that the LCD are on, Default is 1602 Keypad Shield, buttons on A0
const int chipSelect = 53;                 // Chip select (CS) pin for the SD Card, (53)
                                           // Connect SD Card Shield: (MISO = 50, MOSI = 51, SCK = 52)
                                           // If SD card shield has 3.3v regulator then connect 5v, else 3.3v. Don't forget the GND.
//============================================================================================================================================
unsigned int MountedFile4 = 0;              // The current mounted file index number for device 4
unsigned int MountedFile5 = 0;              // The current mounted file index number for device 5
unsigned int MountedFile6 = 0;              // The current mounted file index number for device 6
unsigned int MountedFile7 = 0;              // The current mounted file index number for device 7
byte devicenumber_displayed;
//============================================================================================================================================
const unsigned int maxfiles = 100;         // The maximum number of file names to load into the directory array. This will crash the mega if you make it too big
const unsigned int namelength = 40;        // Length of file name to store in array. Memory used: 100 x 40 = 4000 bytes
const int statusled = 13;                  // Pin that the status LED is on. This is the internal LED on the Mega
const byte interleave = 5;                 // This is the interleave used for the disk image file layout. Not sure if changing it will actually work.
const unsigned int LCDScrollDelay = 300;   // How many milliseconds between scrolls
const unsigned int LCDScrollDelayStart = 2000;// Scroll delay when the LCD gets to the start
unsigned int CurrentLCDDelay = LCDScrollDelay;// The current LCD scroll delay
byte status4[6];                           // Status array for device 4
byte status5[6];                           // Status array for device 5
byte status6[6];                           // Status array for device 6
byte status7[6];                           // Status array for device 7
String sdfiles[maxfiles + 1];              // Buffer for the SD card file names 
unsigned int filesindex[maxfiles + 1];     // Index for the files on the SD card
unsigned int numberoffiles = 0;            // The number of files on the SD card
unsigned int currentfile = 1;              // The current file index number that being displayed
byte blockdata[1028];                      // Block buffer array : 3 byte header + 1024 byte buffer + 1 byte checksum, Checksum is only for the 1024 bytes
int loadedblock = -1;                      // The current block number that is loaded in the buffer array
unsigned int WantedBlock = 0x0000;         // The wanted block number
unsigned long LastByteTime;                // Timer for incoming byte interrupt
unsigned long LastButtonTime;              // Timer for last button push
unsigned long LastCommandTime;             // Timer for last command we processed. This is for the keypress disable while commands are coming in.
unsigned long LastScrollLCD;               // Timer for scrolling the LCD
unsigned int LCDScrollLocation = 0;        // Current location for scrolling the LCD
byte refreshscreen = 1;                    // Flag to refresh the LCD
int keypressIn  = 0;                       // Integer value for the analog keypress
byte IncomingCommand = 0x00;               // Incoming command from AdamNet
byte IncomingCommandFlag = 0;              // Flag for the main loop to process an incoming command
SdFat sd;                                  // Setup SD Card
SdFile file;                               // Setup SD Card
SdFile dirFile;                            // Setup SD Card
void setup(){
  String eepromfilename;
  Serial.begin(115200);
  pinMode(statusled,OUTPUT);               // Set the status LED as output
  digitalWrite(statusled,LOW);             // Turn off the status LED  
  lcd.begin(16, 2);                        // start the LCD screen
  Serial.println("Starting up...");      
  sdfiles[0] = "";                         // filename for no file mounted. Position 0 in the array is no file mounted.
  SDCardSetup();                           // Initialize the SD card and load the root directory
  devicenumber_displayed = EEPROM.read(0);
  if (devicenumber_displayed < 4 || devicenumber_displayed > 7){
   devicenumber_displayed = 4; 
  }
//=====================  Setup Drive D1 (Device 4) ======================================================
  if (4 <= NumberofDrives +3 ){
    MountedFile4 = EEPROM.read(4);
    eepromfilename = EepromStringRead(400);
    if (MountedFile4 > 100 || eepromfilename != sdfiles[MountedFile4]){
      MountedFile4 = 0;
    }
    else{
      Serial.print("D1: Mounting: ");
      Serial.println(eepromfilename);
    }
    if (MountedFile4 == 0){
      StatusSetup(0x43, 4);                     // Setup the Status array to "no disk"
      Serial.println("D1: < No Disk >");
    }
    else{
      StatusSetup(0x40, 4);                    // Set the status to "disk in"
    }
  }
//=====================  Setup Drive D2 (Device 5) ======================================================
  if (5 <= NumberofDrives +3 ){
    MountedFile5 = EEPROM.read(5);
    eepromfilename = EepromStringRead(500);
    if (MountedFile5 > 100 || eepromfilename != sdfiles[MountedFile5]){
      MountedFile5 = 0;
    }
    else{
      Serial.print("D2: Mounting: ");
      Serial.println(eepromfilename);
    }
    if (MountedFile5 == 0){
      StatusSetup(0x43, 5);                     // Setup the Status array to "no disk"
      Serial.println("D2: < No Disk >");
    }
    else{
      StatusSetup(0x40, 5);                    // Set the status to "disk in"
    }
  }
//=====================  Setup Drive D3 (Device 6) ======================================================
  if (6 <= NumberofDrives +3 ){
    MountedFile6 = EEPROM.read(6);
    eepromfilename = EepromStringRead(600);
    if (MountedFile6 > 100 || eepromfilename != sdfiles[MountedFile6]){
      MountedFile6 = 0;
    }
    else{
      Serial.print("D3: Mounting: ");
      Serial.println(eepromfilename);
    }
    if (MountedFile6 == 0){
      StatusSetup(0x43, 6);                     // Setup the Status array to "no disk"
      Serial.println("D3: < No Disk >");    
    }
    else{
      StatusSetup(0x40, 6);                    // Set the status to "disk in"
    }
  }
//=====================  Setup Drive D4 (Device 7) ======================================================
  if (7 <= NumberofDrives +3 ){
    MountedFile7 = EEPROM.read(7);
    eepromfilename = EepromStringRead(700);
    if (MountedFile7 > 100 || eepromfilename != sdfiles[MountedFile7]){
      MountedFile7 = 0;
    }
    else{
      Serial.print("D4: Mounting: ");
      Serial.println(eepromfilename);
    }
    if (MountedFile7 == 0){
      StatusSetup(0x43, 7);                     // Setup the Status array to "no disk"
      Serial.println("D4: < No Disk >");    
    }
    else{
      StatusSetup(0x40, 7);                    // Set the status to "disk in"
    }
  }
//=======================================================================================================
  LastByteTime = micros();                 // Setup initial time for AdamNet bytes in us
  LastButtonTime = millis();               // Setup initial time for Button push in ms
  LastCommandTime = millis();              // Setup initial time for last command in ms
  LastScrollLCD = millis();                // Setup initial time for LCD scrolling
  pinMode(AdamNetPin,INPUT);               // Setup AdamNetPin to input
  EIFR = bit (INTF0);                      // clear flag for interrupt 0
  attachInterrupt(digitalPinToInterrupt(AdamNetPin), IncomingInterrupt, RISING);  // Setup Interrupt
}
void loop(){
  if (IncomingCommandFlag == 1){           // Do we have any commands waiting? This is set by the interrupt.
    noInterrupts();                        // Turn off the interrupts so we can process
    if ((IncomingCommand & 0xF) >= 4 && (IncomingCommand & 0xF) <= NumberofDrives + 3){
      ProcessCommand(IncomingCommand);     // Process the incoming command
      LastCommandTime = millis();          // Reset the Last Command timer
    }
    LastByteTime = micros();               // Reset the timer to watch incoming bytes
    IncomingCommandFlag = 0;               // Reset the incoming command flag
    EIFR = bit (INTF0);                    // clear flag for interrupt 0
    interrupts();                          // Turn on interrupts
  }
  unsigned long CommandDelay = millis() - LastCommandTime; 
  if (CommandDelay >= 500){                // Has it been at least 500 ms since the drive processed a command?
                                           // This ensures that we are not changing files in the middle of processing
                                           // We can also miss commands while the analog pin read is happening.
    ProcessKeys();                         // Has the keypress delay passed and do we have a keypress?
    ScrollLCD();                           // Scroll the bottom line of the LCD
  }
}
