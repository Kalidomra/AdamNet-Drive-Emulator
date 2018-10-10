#include <LiquidCrystal.h>
#include <util/delay.h>
#include <SPI.h>
#include <EEPROM.h>
#include "SdFat.h"
//============================================================================================================================================
//==================================             AdamNet Drive Emulator (ADE)        =========================================================
//============================================================================================================================================
const int AdamNetPin = 21;                 // AdamNet serial communication pin. (21 = INT0 = PD0)
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);       // Pins that the LCD are on, Default is 1602 Keypad Shield, buttons on A0
const int chipSelect = 53;                 // Chip select (CS) pin for the SD Card, (53)
                                           // Connect SD Card Shield: (MISO = 50, MOSI = 51, SCK = 52)
                                           // If SD card shield has 3.3v regulator then connect 5v, else 3.3v. Don't forget the GND.
//============================================================================================================================================
byte devicenumber;                         // Default is 4 --> Drive 1 = 4, Drive 2 = 5, Drive 3 =6, Drive 4 = 7
//============================================================================================================================================
const unsigned int maxfiles = 100;         // The maximum number of file names to load into the directory array. This will crash the mega if you make it too big
const unsigned int namelength = 40;        // Length of file name to store in array. Memory used: 100 x 40 = 4000 bytes
const int statusled = 13;                  // Pin that the status LED is on. This is the internal LED on the Mega
const byte interleave = 5;                 // This is the interleave used for the disk image file layout. Not sure if changing it will actually work.
const unsigned int LCDScrollDelay = 300;   // How many milliseconds between scrolls
const unsigned int LCDScrollDelayStart = 2000;// Scroll delay when the LCD gets to the start
unsigned int CurrentLCDDelay = LCDScrollDelay;// The current LCD scroll delay
byte status[6];                            // Status array
String sdfiles[maxfiles + 1];              // Buffer for the SD card file names 
unsigned int filesindex[maxfiles + 1];     // Index for the files on the SD card
unsigned int numberoffiles = 0;            // The number of files on the SD card
unsigned int currentfile = 1;              // The current file index number that being displayed
unsigned int MountedFile = 0;              // The current mounted file index number
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
  Serial.begin(115200);
  pinMode(statusled,OUTPUT);               // Set the status LED as output
  digitalWrite(statusled,LOW);             // Turn off the status LED  
  lcd.begin(16, 2);                        // start the LCD screen
  Serial.println("Starting up...");      
  devicenumber = EEPROM.read(0);           // Read in the device number from the eeprom
  if (devicenumber < 4 || devicenumber > 7){
    devicenumber = 4;                      // If the eeprom contains an unknown number then default to 4
  }
  sdfiles[0] = "";                         // filename for no file mounted. Position 0 in the array is no file mounted.
  SDCardSetup();                           // Initialize the SD card and load the root directory
  StatusSetup(0x43);                       // Setup the Status array to "no disk"
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
    if ((IncomingCommand & 0xF) == devicenumber){  // Is the command for our device?
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
