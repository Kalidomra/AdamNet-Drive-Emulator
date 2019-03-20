#include <LiquidCrystal.h>
#include <util/delay.h>
#include <SPI.h>
#include <EEPROM.h>
#include <SdFat.h>
//============================================================================================================================================
//==================================             AdamNet Drive Emulator (ADE)   v0.60         ================================================
//============================================================================================================================================
//↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓
//↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓   Only modify the following variables   ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓
//↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓
const unsigned int maxfiles = 300;         // The maximum number of file names to load into the directory index array. (max = 300)
const unsigned int namelength = 100;       // Length of file name to display. 16 will disable scrolling.
const byte statusled[4] = {13,13,13,13};   // Pins for status LED. These can be combined or 1 for each device. 13 is the internal LED on the Mega
const String BootDisk = "boot.dsk";        // Name of disk to auto-mount on D1. This will override the eeprom file. Set to "" for no boot disk
const byte EnableAnalogButtons = true;     // For the 1602 Keypad Shield Buttons, leave this as 'true'. If you are using individual digital buttons set it to 'false'.
const byte AnalogButtonSensitivity = 100;  // This will change the analog button sensitivity. This is a percentage of the default values.
const unsigned int LCDScrollDelay = 300;   // How many milliseconds between scrolls
const unsigned int LCDScrollDelayStart = 2000;// Scroll delay when the LCD gets to the start
const byte EnableFACE = true;              // Do we want the FACE command to work. This will cause problems with disk images greater than 64,205 (0xFACD) blocks
                                           // FACE is the format command. If you write to block 0x0000FACE then the whole disk is erased.
                                           // Setting this to false will disable the FACE command and treat 0x0000FACE as a normal block
const long IODelay = 0;                    // Delay in microseconds the loading and saving of each block. This can help to match the timing of an actual drive.
                                           // A real Adam disk drive is 300000 - 500000 (0.3-0.5 sec), however 0 works for everything I tried.
//↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑
//↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑
long CommandTimeout = 1200;                // Microseconds to wait on an incoming byte to make sure it is a command.
const byte AdamNetRx = 19;                 // AdamNet Receive pin.(19 = INT2 = PD2)
const byte AdamNetTx = 21;                 // AdamNet Transmit pin. (21 = INT0 = PD0)
const byte AdamResetPin = 20;              // Pin for optional output to AdamNet reset line. (20 = INT1 = PD1)
const byte chipSelect = 53;                // Chip select (CS) pin for the SD Card (53),Connect SD Card Shield: (MISO = 50, MOSI = 51, SCK = 52)
                                           // If SD card shield has 3.3v regulator then connect 5v, else 3.3v. Don't forget the GND.
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);       // Pins that the LCD are on, Default is 1602 Keypad Shield, buttons on A0
const byte RightButtonPin = 25;            // Pin for optional digital button.
const byte UpButtonPin = 23;               // Pin for optional digital button.
const byte DownButtonPin = 24;             // Pin for optional digital button.
const byte LeftButtonPin = 26;             // Pin for optional digital button.
const byte SelectButtonPin = 22;           // Pin for optional digital button.
const byte interleave = 5;                 // This is the interleave used for the disk image file layout. Not sure if changing it will actually work.
unsigned int CurrentLCDDelay = LCDScrollDelay;// The current LCD scroll delay
byte Device4;                              // Enable for Device 4
byte Device5;                              // Enable for Device 5
byte Device6;                              // Enable for Device 6
byte Device7;                              // Enable for Device 7
byte CompMode = true;                      // Compatability mode. If true, disables the high word block address.
byte status[4][6];                         // Status array
byte blockdata[4][1028];                   // Block buffer array : 3 byte header + 1024 byte buffer + 1 byte checksum, Checksum is only for the 1024 bytes
byte devicenumber_displayed;               // The currently displayed device number on the top row of the LCD
unsigned long loadedblock[4] = {0xFFFFFFFF,
                               0xFFFFFFFF,
                               0xFFFFFFFF,
                               0xFFFFFFFF};// The current block number that is loaded in the buffer array
unsigned int MountedFile[4] = {0,0,0,0};   // The current mounted file index number
unsigned int filesindex[maxfiles + 1];     // Index for the files on the SD card
byte typeindex[maxfiles + 1];              // Index for the file type: 0 = DSK, 1 = DDP
unsigned int bootdiskindex = 0;            // Index number on the SD card for the D1 bootdisk
unsigned int numberoffiles = 0;            // The number of files on the SD card
unsigned int currentfile = 1;              // The current file index number that being displayed
unsigned long WantedBlock = 0x00000000;    // The wanted block number
unsigned long LastByteTime;                // Timer for incoming byte interrupt
unsigned long LastButtonTime;              // Timer for last button push
unsigned long LastCommandTime;             // Timer for last command we processed. This is for the keypress disable while commands are coming in.
unsigned long LastScrollLCD;               // Timer for scrolling the LCD
String LCDCurrentText;                     // Current Text on the bottom line of the LCD
byte LCDScrollOn = true;                   // Turn off the scroll in the program. Used when writing to LCD. Turned back on with refreshscreen.
unsigned int LCDScrollLocation = 0;        // Current location for scrolling the LCD
byte refreshscreen = 1;                    // Flag to refresh the LCD
byte IncomingCommand = 0x00;               // Incoming command from AdamNet
byte IncomingCommandFlag = 0;              // Flag for the main loop to process an incoming command
byte DisableNextReset = false;             // When set to true the next reset will not reset the devices.
byte ProcessKeysDelay = 150;               // How long to wait before processing the next keypress.
int AnalogTriggerRight = 50.0*(AnalogButtonSensitivity/100.0);
int AnalogTriggerUp = 250.0*(AnalogButtonSensitivity/100.0);
int AnalogTriggerDown = 450.0*(AnalogButtonSensitivity/100.0);
int AnalogTriggerLeft = 650.0*(AnalogButtonSensitivity/100.0);
int AnalogTriggerSelect = 850.0*(AnalogButtonSensitivity/100.0);
SdFat sd;                                  // Setup SD Card
SdFile file;                               // Setup SD Card
void setup(){
  Serial.begin(115200);
  lcd.begin(16, 2);                        // Start the LCD screen
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(F("ADE        v0.60"));
  lcd.setCursor(0,1);  
  lcd.print(F("by: Sean Myers"));
  delay(2000);
  Serial.println(F("Starting: ADE v0.60"));
  if (!EnableAnalogButtons){               // If the analog buttons are turned off then we have a different value
    ProcessKeysDelay = 120;
  }
  for(int t=0; t<=3;t++){
    pinMode(statusled[t],OUTPUT);          // Set the status LED's as output
    digitalWrite(statusled[t],LOW);        // Turn off the status LED  
  }
  pinMode(RightButtonPin,INPUT_PULLUP);    // Set the RightButtonPin to Input Pullup
  pinMode(UpButtonPin,INPUT_PULLUP);       // Set the UpButtonPin to Input Pullup
  pinMode(DownButtonPin,INPUT_PULLUP);     // Set the DownButtonPin to Input Pullup
  pinMode(LeftButtonPin,INPUT_PULLUP);     // Set the LeftButtonPin to Input Pullup
  pinMode(SelectButtonPin,INPUT_PULLUP);   // Set the SelectButtonPin to Input Pullup
  int InitReadKeys= 1000;
  if (EnableAnalogButtons){
    InitReadKeys = analogRead(0);
  }
  if (digitalRead(UpButtonPin) == LOW || (InitReadKeys < AnalogTriggerUp)){
  }
  if (digitalRead(SelectButtonPin) == LOW || (InitReadKeys < AnalogTriggerSelect && InitReadKeys > AnalogTriggerLeft)){
    Serial.println(F("Entering Configuration Mode"));
    ConfigMode();
  }
  if (digitalRead(RightButtonPin) == LOW || InitReadKeys < AnalogTriggerRight){
    Serial.println(F("Entering Voltage Test Mode"));
    VoltageRead();
  }
  SDCardSetup();                           // Initialize the SD card and load the root directory
  DeviceSetup();                           // Initialize the Device and Device Display
  Serial.print(F("Free SRAM: "));          // Print the amount of Free SRAM
  Serial.println(FreeMemory());
  LastByteTime = micros();                 // Setup initial time for AdamNet bytes in us
  LastButtonTime = millis();               // Setup initial time for Button push in ms
  LastCommandTime = millis();              // Setup initial time for last command in ms
  LastScrollLCD = millis();                // Setup initial time for LCD scrolling
  pinMode(AdamNetRx,INPUT_PULLUP);         // Setup AdamNetRx to input
  pinMode(AdamNetTx, OUTPUT);              // Setup AdamNetTx to Ouput
  digitalWrite(AdamNetTx, HIGH);           // Set the AdamNetTx to High
  pinMode(AdamResetPin,INPUT_PULLUP);      // Set the Adam reset to input
  EIFR = bit (INTF2);                      // clear flag for interrupt 2 (AdamNet Receive)
  EIFR = bit (INTF1);                      // clear flag for interrupt 1 (AdamNet Reset)
  attachInterrupt(digitalPinToInterrupt(AdamNetRx), CommandInterrupt, RISING);  // Setup the Receive Interrupt
  attachInterrupt(digitalPinToInterrupt(AdamResetPin), ResetInterrupt, FALLING);  // Setup the Reset Interrupt
}
void loop(){
  unsigned long CommandDelay;
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
    LastByteTime = micros();               // Reset the timer to watch incoming bytes
    IncomingCommandFlag = 0;               // Reset the incoming command flag
    EIFR = bit (INTF2);                    // Clear flag for interrupt 2 (AdamNet Receive)
    interrupts();                          // Turn on interrupts
  }
  CommandDelay = millis() - LastCommandTime; 
  if (CommandDelay >= 500){                // Has it been at least 500 ms since the drive processed a command?
                                           // This ensures that we are not changing files in the middle of processing
                                           // We can also miss commands while the analog pin read is happening.
    ProcessKeys();                         // Has the keypress delay passed and do we have a keypress?
    if (LCDScrollOn){
      ScrollLCD();                         // Scroll the bottom line of the LCD
    }
  }
}
