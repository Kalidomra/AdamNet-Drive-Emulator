#include <LiquidCrystal.h>
#include <util/delay.h>
#include <SPI.h>
#include <EEPROM.h>
#include <SdFat.h>
//============================================================================================================================================
//==================================             AdamNet Drive Emulator (ADE)   v0.3         =================================================
//============================================================================================================================================
//↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓
//↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓   Only modify the following variables   ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓
//↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓
const byte Device4 = true;                 // Set to 'true' if you want to enable Device 4 (Disk Drive 1), 'false' to disable
const byte Device5 = true;                 // Set to 'true' if you want to enable Device 5 (Disk Drive 2), 'false' to disable
const byte Device6 = true;                 // Set to 'true' if you want to enable Device 6 (Disk Drive 3), 'false' to disable
const byte Device7 = true;                 // Set to 'true' if you want to enable Device 7 (Disk Drive 4), 'false' to disable
const unsigned int maxfiles = 400;         // The maximum number of file names to load into the directory index array.
const unsigned int namelength = 100;       // Length of file name to display. 16 will disable scrolling.
const int statusled[4] = {13,13,13,13};    // Pins for status LED. These can be combined or 1 for each device. 13 is the internal LED on the Mega
//↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑
//↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑
const int AdamNetPin = 21;                 // AdamNet serial communication pin. (21 = INT0 = PD0).
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);       // Pins that the LCD are on, Default is 1602 Keypad Shield, buttons on A0
const int chipSelect = 53;                 // Chip select (CS) pin for the SD Card, (53)
                                           // Connect SD Card Shield: (MISO = 50, MOSI = 51, SCK = 52)
                                           // If SD card shield has 3.3v regulator then connect 5v, else 3.3v. Don't forget the GND.
unsigned int MountedFile[4] = {0,0,0,0};   // The current mounted file index number
byte devicenumber_displayed;               // The currently displayed device number on the top row of the LCD
const byte interleave = 5;                 // This is the interleave used for the disk image file layout. Not sure if changing it will actually work.
const unsigned int LCDScrollDelay = 300;   // How many milliseconds between scrolls
const unsigned int LCDScrollDelayStart = 2000;// Scroll delay when the LCD gets to the start
unsigned int CurrentLCDDelay = LCDScrollDelay;// The current LCD scroll delay
byte status[4][6];                         // Status array
byte blockdata[4][1028];                   // Block buffer array : 3 byte header + 1024 byte buffer + 1 byte checksum, Checksum is only for the 1024 bytes
int loadedblock[4] = {-1,-1,-1,-1};        // The current block number that is loaded in the buffer array
unsigned int filesindex[maxfiles + 1];     // Index for the files on the SD card
byte typeindex[maxfiles + 1];      // Index for the file type: 0 = DSK, 1 = DPP
unsigned int numberoffiles = 0;            // The number of files on the SD card
unsigned int currentfile = 1;              // The current file index number that being displayed
unsigned int WantedBlock = 0x0000;         // The wanted block number
unsigned long LastByteTime;                // Timer for incoming byte interrupt
unsigned long LastButtonTime;              // Timer for last button push
unsigned long LastCommandTime;             // Timer for last command we processed. This is for the keypress disable while commands are coming in.
unsigned long LastScrollLCD;               // Timer for scrolling the LCD
String LCDCurrentText;                     // Current Text on the bottom line of the LCD
unsigned int LCDScrollLocation = 0;        // Current location for scrolling the LCD
byte refreshscreen = 1;                    // Flag to refresh the LCD
int keypressIn  = 0;                       // Integer value for the analog keypress
byte IncomingCommand = 0x00;               // Incoming command from AdamNet
byte IncomingCommandFlag = 0;              // Flag for the main loop to process an incoming command
SdFat sd;                                  // Setup SD Card
SdFile file;                               // Setup SD Card
void setup(){
  Serial.begin(115200);
  for(int t = 0; t<=3;t++){
    pinMode(statusled[t],OUTPUT);          // Set the status LED as output
    digitalWrite(statusled[t],LOW);        // Turn off the status LED  
  }
  lcd.begin(16, 2);                        // Start the LCD screen
  Serial.println("Starting up...");      
  SDCardSetup();                           // Initialize the SD card and load the root directory
  DeviceSetup();                           // Initialize the Device and Device Display
  Serial.print("Free SRAM: ");             // Print the amount of Free SRAM
  Serial.println(freeMemory());
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
    byte WantedDevice = IncomingCommand & 0xF;
    if (WantedDevice == 4 && Device4){
      ProcessCommand(IncomingCommand, WantedDevice);// Process the incoming command for Device 4
      LastCommandTime = millis();          // Reset the Last Command timer
    }
    else if (WantedDevice == 5 && Device5){
      ProcessCommand(IncomingCommand, WantedDevice);// Process the incoming command for Device 5
      LastCommandTime = millis();          // Reset the Last Command timer
    }
    else if (WantedDevice == 6 && Device6){
      ProcessCommand(IncomingCommand, WantedDevice);// Process the incoming command for Device 6
      LastCommandTime = millis();          // Reset the Last Command timer
    }
    else if (WantedDevice == 7 && Device7){
      ProcessCommand(IncomingCommand, WantedDevice);// Process the incoming command for Device 7
      LastCommandTime = millis();          // Reset the Last Command timer
    }
 /*
    else if (WantedDevice != 1){
      Serial.print("Command: ");
      Serial.println(IncomingCommand,HEX);
    }
 */   
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
