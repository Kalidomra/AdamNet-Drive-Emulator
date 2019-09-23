void ProcessKeys(){                                                // Process a Button Press
  unsigned long ButtonDelay = millis() - LastButtonTime;
  byte RightButton;
  byte UpButton;
  byte DownButton;
  byte LeftButton;
  byte SelectButton;
  byte SwapButton;
  int keypressIn  = 1000;
  if (ButtonDelay >= ProcessKeysDelay){    // Only process the key inputs every so often.
    if (EnableAnalogButtons){
      keypressIn = analogRead(0);          // Read Analog pin 0 (Takes 110 us)
    }
    RightButton = digitalRead(RightButtonPin);
    UpButton = digitalRead(UpButtonPin);
    DownButton = digitalRead(DownButtonPin);
    LeftButton = digitalRead(LeftButtonPin);
    SelectButton = digitalRead(SelectButtonPin);
    SwapButton = digitalRead(SwapButtonPin);
    if (((keypressIn < AnalogTriggerRight) || (RightButton == LOW)) && (RepeatKeyFlag == 0)){        // Right -->  Mount or Unmount the Currently Selected Disk Image
        unsigned long KeyTimer = 0;
        while(((keypressIn < AnalogTriggerRight) || (RightButton == LOW)) && (KeyTimer < LongKey)){
          if (EnableAnalogButtons){
            keypressIn = analogRead(0);    // Read Analog pin 0 (Takes 110 us)
          }
          RightButton = digitalRead(RightButtonPin);
          KeyTimer++;
        }
        if (KeyTimer < LongKey){           // Short Key Press
            sd.chdir(CurrentDirectory.c_str(),true);
            RepeatKeyFlag = 1;
            if ((TypeIndex[CurrentFile] == 10) || (TypeIndex[CurrentFile] == 11)|| (TypeIndex[CurrentFile] == 12)
               || (TypeIndex[CurrentFile] == 22) || (TypeIndex[CurrentFile] == 23) || (TypeIndex[CurrentFile] == 24)){   // This is a DSK, DDP, ROM, DS2, DS3 or DS4 file
              String FileNametoMount = GetFileName(CurrentFile,99);      // Mount
              Serial.print(F("Mounting D"));
              Serial.print(DeviceDisplayed - 3);
              Serial.print(F(": "));
              Serial.println(FileNametoMount);
              LoadedBlock[DeviceDisplayed-4] = 0xFFFFFFFF; // Reset the loaded block for the new file
              StatusSetup(0x40,DeviceDisplayed); // Set the Status to 'disk in"
              EEPROM.put((DeviceDisplayed * 400), CurrentFile);                // Write the File Number
              EEPROM.put((DeviceDisplayed * 400) + 2, FilesIndex[CurrentFile]);// Write the File Number Index
              EEPROM.put((DeviceDisplayed * 400) + 4, TypeIndex[CurrentFile]); // Write the File Type
              EepromStringWrite((DeviceDisplayed * 400) + 5, FileNametoMount);
              EepromStringWrite((DeviceDisplayed * 400) + 105, CurrentDirectory);
              BootDiskMounted = 0;
              LCDTopDelay = 1;
            }
            else if (TypeIndex[CurrentFile] == 0){                             // This is a go back to root. [/]
              CurrentDirectory = "/";
              Serial.print(F("Changing to Directory: "));
              Serial.println(CurrentDirectory);
              sd.chdir(CurrentDirectory.c_str(),true);
              lcd.setCursor(0,0);
              lcd.print(F("                "));
              lcd.setCursor(0,0);
              lcd.print(CurrentDirectory);
              LCDTopDelay = LCDDelay;
              lcd.setCursor(0,1);
              lcd.print(F("Loading Dir...  "));
              SDCardGetDir();
              CurrentFile = 1;
              LCDBottomDelay = 1;
            }
            else if (TypeIndex[CurrentFile] == 1){                             // This is the previous directory [..]
              CurrentDirectory.remove(CurrentDirectory.length()-1,1);
              int templast = CurrentDirectory.lastIndexOf("/");
              if (templast <= 0){
                CurrentDirectory = "/";
                templast = 1;
              }
              else{
                CurrentDirectory = CurrentDirectory.substring(0,templast);
              }          
              Serial.print(F("Changing to Directory: "));
              Serial.println(CurrentDirectory);
              sd.chdir(CurrentDirectory.c_str(),true);
              lcd.setCursor(0,0);
              lcd.print(F("                "));
              lcd.setCursor(0,0);
              lcd.print(CurrentDirectory);
              LCDTopDelay = LCDDelay;
              lcd.setCursor(0,1);
              lcd.print(F("Loading Dir...  "));
              SDCardGetDir();
              SetCurrentFile();
              LCDBottomDelay = 1;
            }
            else if (TypeIndex[CurrentFile] == 2){                             // This is a directory
              String tempdir = GetFileName(CurrentFile,99);
              if (CurrentDirectory == "/"){
                CurrentDirectory = CurrentDirectory + tempdir.substring(1, tempdir.length()-1);
              }
              else{
                CurrentDirectory = CurrentDirectory + "/" + tempdir.substring(1, tempdir.length()-1);
              }
              Serial.print(F("Changing to Directory: "));
              Serial.println(CurrentDirectory);
              sd.chdir(CurrentDirectory.c_str(),true);
              lcd.setCursor(0,0);
              lcd.print(F("                "));
              lcd.setCursor(0,0);
              lcd.print(CurrentDirectory);
              LCDTopDelay = LCDDelay;
              lcd.setCursor(0,1);
              lcd.print(F("Loading Dir...  "));
              SDCardGetDir();
              SetCurrentFile();
              LCDBottomDelay = 1;
            }
        }
        else{                                  // Unmount - Long Key Press
          if ((EEPROM[DeviceDisplayed * 400] != 0) || (BootDiskMounted == 1)){                     
            Serial.print(F("Unmounting D"));
            Serial.println(DeviceDisplayed - 3);
            StatusSetup(0x43,DeviceDisplayed); // Set the Status to "no disk"
            EEPROM.write((DeviceDisplayed * 400), 0);          // Write the File Number
            EEPROM.write((DeviceDisplayed * 400) + 2, 0);      // Write the File Number Index
            EEPROM.write((DeviceDisplayed * 400) + 4, 0);      // Write the File Type = 0
            EepromStringWrite((DeviceDisplayed * 400) + 5, "");// Write the File Name = "" 
            EepromStringWrite((DeviceDisplayed * 400) + 105, "");  // Write the current directory = ""
            BootDiskMounted = 0;
          } 
          LCDTopDelay = 1;
          RepeatKeyFlag = 1;
        }
    }
    else if ((keypressIn < AnalogTriggerUp && keypressIn > AnalogTriggerRight) || UpButton == LOW){  // Up -->     Scroll Up in the List
      if (CurrentFile == 1){
        CurrentFile = NumberofFiles;
      }
      else {
        CurrentFile--;
      }
      LCDBottomDelay = 1;
    }
    else if ((keypressIn < AnalogTriggerDown  && keypressIn > AnalogTriggerUp)|| DownButton == LOW){ // Down -->   Scroll Down in the List
      if (CurrentFile == NumberofFiles){
        CurrentFile = 1;
      }
      else {
        CurrentFile++;
      }
      LCDBottomDelay = 1;
    }
    else if (((keypressIn < AnalogTriggerLeft) || (LeftButton == LOW)) && (RepeatKeyFlag == 0)){     // Left -->   Move Back Directory (Long Press = Move to root)
      
      unsigned long KeyTimer = 0;
      while(((keypressIn < AnalogTriggerLeft) || (LeftButton == LOW)) && (KeyTimer < LongKey)){
        if (EnableAnalogButtons){
          keypressIn = analogRead(0);          // Read Analog pin 0 (Takes 110 us)
        }
        SelectButton = digitalRead(SelectButtonPin);
        KeyTimer++;
      }
      if (KeyTimer < LongKey){             // Short Key Press
        RepeatKeyFlag = 1;
        CurrentDirectory.remove(CurrentDirectory.length()-1,1);
        int templast = CurrentDirectory.lastIndexOf("/");
        if (templast <= 0){
         CurrentDirectory = "/";
         templast = 1;
        }
        else{
          CurrentDirectory = CurrentDirectory.substring(0,templast);
        }          
        Serial.print(F("Changing to Directory: "));
        Serial.println(CurrentDirectory);
        sd.chdir(CurrentDirectory.c_str(),true);
        lcd.setCursor(0,0);
        lcd.print(F("                "));
        lcd.setCursor(0,0);
        lcd.print(CurrentDirectory);
        LCDTopDelay = LCDDelay;
        lcd.setCursor(0,1);
        lcd.print(F("Loading Dir...  "));
        SDCardGetDir();
        SetCurrentFile();
        LCDBottomDelay = 1;
      }
      else{                                //Long Key Press
        CurrentDirectory = "/";
        Serial.print(F("Changing to Directory: "));
        Serial.println(CurrentDirectory);
        sd.chdir(CurrentDirectory.c_str(),true);
        lcd.setCursor(0,0);
        lcd.print(F("                "));
        lcd.setCursor(0,0);
        lcd.print(CurrentDirectory);
        LCDTopDelay = LCDDelay;
        lcd.setCursor(0,1);
        lcd.print(F("Loading Dir...  "));
        SDCardGetDir();
        SetCurrentFile();
        LCDBottomDelay = 1;
      }
    }
    else if (((keypressIn < AnalogTriggerSelect) || (SelectButton == LOW)) && (RepeatKeyFlag == 0)){ // Select --> Change the Displayed Device Number (Long Press = Disable/Enable Boot Disk)
      unsigned long KeyTimer = 0;
      while(((keypressIn < AnalogTriggerSelect) || (SelectButton == LOW)) && (KeyTimer < LongKey)){
        if (EnableAnalogButtons){
          keypressIn = analogRead(0);      // Read Analog pin 0
        }
        SelectButton = digitalRead(SelectButtonPin);
        KeyTimer++;
      }
      if (KeyTimer < LongKey){             // Short Key Press
        RepeatKeyFlag = 1;
        switch(DeviceDisplayed){
          case 4:
            if(Device5){
              DeviceDisplayed = 5;
            }
            else if(Device6){
              DeviceDisplayed = 6;
            }
            else if(Device7){
              DeviceDisplayed = 7;
            }
            else {
              DeviceDisplayed = 4;
            }
            break;
          case 5:
            if(Device6){
              DeviceDisplayed = 6;
            }
            else if(Device7){
              DeviceDisplayed = 7;
            }
            else if(Device4){
              DeviceDisplayed = 4;
            }
            else {
              DeviceDisplayed = 5;
            }
            break;
          case 6:
            if(Device7){
              DeviceDisplayed = 7;
            }
            else if(Device4){
              DeviceDisplayed = 4;
            }
            else if(Device5){
              DeviceDisplayed = 5;
            }
            else {
              DeviceDisplayed = 6;
            }
            break;
          case 7:
            if(Device4){
              DeviceDisplayed = 4;
            }
            else if(Device5){
              DeviceDisplayed = 5;
            }
            else if(Device6){
              DeviceDisplayed = 6;
            }
            else {
              DeviceDisplayed = 7;
            }
            break;
        }
        EEPROM.write(3,DeviceDisplayed);
        LCDTopDelay = 1;
     }
      else{                                //Long Key Press
       if ((BootDiskEnabled == 1) && (BootDiskExists == 1)){
         BootDiskEnabled = 0;
         lcd.setCursor(0,0);
         lcd.print(F("Boot Disk Off   "));
         Serial.println(F("Boot Disk Off"));
       }
       else if ((BootDiskEnabled == 0) && (BootDiskExists == 1)){
         BootDiskEnabled = 1;
         lcd.setCursor(0,0);
         lcd.print(F("Boot Disk On    "));
         Serial.println(F("Boot Disk On"));
       }
       else{
         lcd.setCursor(0,0);
         lcd.print(F("No Boot Disk    "));
       }
       RepeatKeyFlag = 1;
       LCDTopDelay = LCDDelay;  
     }
    }
    else if ((SwapButton == LOW) && (RepeatKeyFlag == 0)){                                           // Swap --> Swap the disks on Device 4
      String filedir;
      unsigned int filenumber;
      byte filetype;
      unsigned int FileNumbertoMount;
      byte NumberofBlinks = 0;
      filedir = EepromStringRead((4 * 400) + 105);
      EEPROM.get((4 * 400), filenumber);
      filetype = EEPROM[(4 * 400) + 4];
      if (filenumber != 0){
        if (filetype == 10){
          FileNumbertoMount = GetFileNumber(filenumber,filedir,".ds2");
          NumberofBlinks = 2;
          if (FileNumbertoMount == 0){
            FileNumbertoMount = GetFileNumber(filenumber,filedir,".dsk");
            NumberofBlinks = 1;
          }
        }
        else if (filetype == 22){
          FileNumbertoMount = GetFileNumber(filenumber,filedir,".ds3");
          NumberofBlinks = 3;
          if (FileNumbertoMount == 0){
            FileNumbertoMount = GetFileNumber(filenumber,filedir,".dsk");
            NumberofBlinks = 1;
          }
        }
        else if (filetype == 23){
          FileNumbertoMount = GetFileNumber(filenumber,filedir,".ds4");
          NumberofBlinks = 4;
          if (FileNumbertoMount == 0){
            FileNumbertoMount = GetFileNumber(filenumber,filedir,".dsk");
            NumberofBlinks = 1;
          }
        }
        else if (filetype == 24){
          FileNumbertoMount = GetFileNumber(filenumber,filedir,".dsk");
          NumberofBlinks = 1;
        }
        if (FileNumbertoMount != 0 ){
          String FileNametoMount = GetFileName(FileNumbertoMount,99);
          Serial.print(F("Mounting D1: "));
          Serial.println(FileNametoMount);
          LoadedBlock[0] = 0xFFFFFFFF;      // Reset the loaded block for the new file
          StatusSetup(0x40,4);             // Set the Status to 'disk in"
          EEPROM.put((4 * 400), FileNumbertoMount); // Write the File Number
          EEPROM.put((4 * 400) + 2, FilesIndex[FileNumbertoMount]);// Write the File Number Index
          EEPROM.put((4 * 400) + 4, TypeIndex[FileNumbertoMount]); // Write the File Type
          EepromStringWrite((4 * 400) + 5, FileNametoMount);
          EepromStringWrite((4 * 400) + 105, filedir);
          BootDiskMounted = 0;
          LCDTopDelay = 1;
          for (int blinks = 1; blinks <= NumberofBlinks; blinks++){
            delay(200);
            digitalWrite(StatusLed[0], !digitalRead(StatusLed[0]));
            delay(200);
            digitalWrite(StatusLed[0], !digitalRead(StatusLed[0]));
          }
        }
      }
      RepeatKeyFlag = 1;
    }
    else if ((keypressIn > AnalogTriggerSelect) && (RightButton == HIGH) && (UpButton == HIGH)
              && (DownButton == HIGH) && (LeftButton == HIGH) && (SelectButton == HIGH) 
              && (SwapButton == HIGH)){                                                              // Reset the Repeat Key Flag
      RepeatKeyFlag = 0;
    }
    LastButtonTime = millis();             // Reset the button press timer
  }
}
