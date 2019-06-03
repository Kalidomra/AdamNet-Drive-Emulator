#include <util/delay.h>
#include <SPI.h>
#include <EEPROM.h>
#include <SdFat.h>
#include <LiquidCrystal.h>
//============================================================================================================================================
//==================================             AdamNet Drive Emulator (ADE)   v0.73         ================================================
//============================================================================================================================================
//↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓
//↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓   Only modify the following variables   ↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓
//↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓
const byte Version[3] =  {0,7,3};          // ADE Version Number
const unsigned int MaxFiles = 300;         // The maximum number of file names to load into the directory index array. (max = 300)
const byte NameLength = 100;               // Maximum length of file name to display. 16 will disable scrolling. (max = 100)
const byte StatusLed[4] = {13,13,13,13};   // Pins for Status LED. These can be combined or 1 for each device. 13 is the internal LED on the Mega
const String BootDisk = "boot.dsk";        // Name of disk to auto-mount on D1. This will override the eeprom file. Set to "" for no boot disk
const byte EnableAnalogButtons = true;     // For the 1602 Keypad Shield Buttons, leave this as 'true'. If you are using individual digital buttons set it to 'false'.
const byte AnalogButtonSensitivity = 100;  // This will change the analog button sensitivity. This is a percentage of the default values.
const unsigned int LCDScrollDelay = 300;   // How many milliseconds between scrolls
const unsigned int LCDScrollDelayStart = 2000;// Scroll delay when the LCD gets to the start
const byte EnableFACE = true;              // True will enable the default Adam FACE command for formatting.
                                           // True will cause problems with disk images greater than 64,205 (0xFACD) blocks.
                                           // False will disable the FACE command and treat 0x0000FACE as a normal block.
                                           // False will not allow normal Adam format programs to function.
const long IODelay = 0;                    // Delay in microseconds the loading and saving of each block. This can help to match the timing of an actual drive.
                                           // A real Adam disk drive is 300,000 - 500,000 (0.3-0.5 sec), however 0 works for everything I tried.
//↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑
//↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑
const byte AdamNetRx = 19;                 // AdamNet Receive pin.(19 = INT2 = PD2). Do not change this. It is hard coded.
const byte AdamNetTx = 21;                 // AdamNet Transmit pin. (21 = INT0 = PD0). Do not change this. It is hard coded.
const byte AdamResetPin = 20;              // Pin for optional output to AdamNet reset line. (20 = INT1 = PD1). Do not change this. It is hard coded.
const byte ChipSelect = 53;                // Chip select (CS) pin for the SD Card (53),Connect SD Card Shield: (MISO = 50, MOSI = 51, SCK = 52)
                                           // If SD card shield has 3.3v regulator then connect 5v, else 3.3v. Don't forget the GND.
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);       // Pins that the LCD are on, Default is 1602 Keypad Shield, buttons are on A0
const int RightButtonPin = 25;             // Pin for optional digital button.
const int UpButtonPin = 23;                // Pin for optional digital button.
const int DownButtonPin = 24;              // Pin for optional digital button.
const int LeftButtonPin = 26;              // Pin for optional digital button.
const int SelectButtonPin = 22;            // Pin for optional digital button.
const byte Interleave = 5;                 // This is the Interleave used for the disk image file layout. Not sure if changing it will actually work.
unsigned int CurrentLCDDelay = LCDScrollDelay;// The current LCD scroll delay
byte Device4;                              // Enable for Device 4
byte Device5;                              // Enable for Device 5
byte Device6;                              // Enable for Device 6
byte Device7;                              // Enable for Device 7
byte CompMode = true;                      // Compatability mode. If true, disables the high word block address.
byte Status[4][6];                         // Status array
byte BlockBuffer[4][1028];                 // Block buffer array : 3 byte header + 1024 byte buffer + 1 byte checksum, Checksum is only for the 1024 bytes
byte DeviceDisplayed;                      // The currently displayed device number on the top row of the LCD
unsigned long LoadedBlock[4] = {0xFFFFFFFF,
                               0xFFFFFFFF,
                               0xFFFFFFFF,
                               0xFFFFFFFF};// The current block number that is loaded in the buffer array

unsigned int MountedFile[4] = {0,0,0,0};   // The current mounted file index number
unsigned int FilesIndex[MaxFiles + 1];     // Index for the files on the SD card
byte typeindex[MaxFiles + 1];              // Index for the file type: 0 = DSK, 1 = DDP
unsigned int BootDiskIndex = 0;            // Index number on the SD card for the D1 bootdisk
unsigned int NumberofFiles = 0;            // The number of files on the SD card
unsigned int CurrentFile = 1;              // The current file index number that being displayed
unsigned long LastButtonTime;              // Timer for last button push
unsigned long LastCommandTime;             // Timer for last command processed. This is for the keypress disable while commands are coming in.
unsigned long LastScrollLCD;               // Timer for scrolling the LCD
unsigned long TimetoByte = 0xFFFFFFFF;     // Timer for reset, used to determine Hard or Soft reset
String LCDCurrentText;                     // Current Text on the bottom line of the LCD
byte LCDScrollOn = true;                   // Turn off the scroll in the program. Used when writing to LCD. Turned back on with refreshscreen.
unsigned int LCDScrollLocation = 0;        // Current location for scrolling the LCD
byte refreshscreen = 1;                    // Flag to refresh the LCD
unsigned long WantedBlock = 0x00000000;    // The wanted block number
byte WantedDevice = 0x00;                  // The wanted device for the wanted block
byte LoadBufferArrayFlag = 0;              // Flag for the main loop to load the buffer with the wanted block
byte SaveBufferArrayFlag = 0;              // Flag for the main loop to save the buffer in the wanted block
byte ResetFlag = 0;                        // Flag for the main loop to process a reset interrupt
byte DisableNextReset = false;             // When set to true the next reset will not reset the devices.
byte ProcessKeysDelay = 120;               // How long to wait before processing the next keypress.
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
  lcd.print(F("ADE      v"));
  lcd.print(String(Version[0]) + "." + String(Version[1]) + String(Version[2]));
  lcd.setCursor(0,1);  
  lcd.print(F("by: Sean Myers"));
  delay(2000);
  Serial.print(F("Starting: ADE v"));
  Serial.println(String(Version[0]) + "." + String(Version[1]) + String(Version[2]));
  for(int t=0; t<=3;t++){
    pinMode(StatusLed[t],OUTPUT);          // Set the Status LED's as output
    digitalWrite(StatusLed[t],LOW);        // Turn off the Status LED  
  }
  pinMode(RightButtonPin,INPUT_PULLUP);    // Set the RightButtonPin to Input Pullup
  pinMode(UpButtonPin,INPUT_PULLUP);       // Set the UpButtonPin to Input Pullup
  pinMode(DownButtonPin,INPUT_PULLUP);     // Set the DownButtonPin to Input Pullup
  pinMode(LeftButtonPin,INPUT_PULLUP);     // Set the LeftButtonPin to Input Pullup
  pinMode(SelectButtonPin,INPUT_PULLUP);   // Set the SelectButtonPin to Input Pullup
  int InitReadKeys= 1000;
  if (EnableAnalogButtons){
    InitReadKeys = analogRead(0);          // Read the analog buttons
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
  LastCommandTime = millis();              // Setup initial time for last command in ms
  LastScrollLCD = millis();                // Setup initial time for LCD scrolling
  TimetoByte = millis();                   // Setup initial time for Reset Timing
  pinMode(AdamNetRx,INPUT_PULLUP);         // Setup AdamNetRx to input
  pinMode(AdamNetTx, OUTPUT);              // Setup AdamNetTx to Ouput
  digitalWrite(AdamNetTx, HIGH);           // Set the AdamNetTx to High
  pinMode(AdamResetPin,INPUT_PULLUP);      // Set the Adam reset to input
  EIFR = bit (INTF2);                      // Clear flag for interrupt 2 (AdamNet Receive)
  EIFR = bit (INTF1);                      // Clear flag for interrupt 1 (AdamNet Reset)
  attachInterrupt(digitalPinToInterrupt(AdamNetRx), CommandInterrupt, RISING);  // Setup the Receive Interrupt
  attachInterrupt(digitalPinToInterrupt(AdamResetPin), ResetInterrupt, FALLING);  // Setup the Reset Interrupt
}
void loop(){
  if (SaveBufferArrayFlag == 1){           // Is there a pending buffer save?
    noInterrupts();                        // Disable Interrupts
    BufferSaveBlock();                     // Process the save command
    LastCommandTime = millis();            // Reset the Last Command timer
    AdamNetIdle();                         // Wait for AdamNet to go Idle
    EIFR = bit (INTF2);                    // Clear flag for interrupt 2 (AdamNet Receive)
    interrupts();                          // Enable Interrupts
    SaveBufferArrayFlag = 0;               // Reset the flag
  }
  if (LoadBufferArrayFlag == 1){           // Is there a pending buffer load?
    noInterrupts();                        // Disable Interrupts
    BufferLoadBlock();                     // Process the load command
    LastCommandTime = millis();            // Reset the Last Command timer
    AdamNetIdle();                         // Wait for AdamNet to go Idle
    EIFR = bit (INTF2);                    // Clear flag for interrupt 2 (AdamNet Receive)
    interrupts();                          // Enable Interrupts
    LoadBufferArrayFlag = 0;               // Reset the flag
  }
  if (ResetFlag == 1){
    ProcessReset();                        // Process the Reset
    LastCommandTime = millis();            // Reset the Last Command timer
    AdamNetIdle();                         // Wait for AdamNet to go Idle
    ResetFlag = 0;                         // Reset the flag
    refreshscreen = 1;
  } 
  if ((millis() - LastCommandTime) >= 500){ //Has it been at least 500 ms since the drive processed a command?
    ProcessKeys();                         // Has the keypress delay passed and is there a keypress?
    if (LCDScrollOn){
      ScrollLCD();                         // Scroll the bottom line of the LCD
    }
  }
}
