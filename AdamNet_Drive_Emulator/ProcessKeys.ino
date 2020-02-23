void ProcessKeys(){                                                // Process a Button Press
  byte ButtonPressed = ReadButtons();
  if (ButtonPressed == 1){                 // Right (Short) -->  Mount the currently selected disk image OR move to selected dir OR move back one directory
    sd.chdir(CurrentDirectory.c_str(),true);
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
  else if (ButtonPressed == 91){           // Right (Long) -->  Unmount the image from the selected drive
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
  }
  else if (ButtonPressed == 2){            // Up -->  Scroll Up in the List
    if (CurrentFile == 1){
      CurrentFile = NumberofFiles;
    }
    else {
      CurrentFile--;
    }
    LCDBottomDelay = 1;
  }
  else if (ButtonPressed == 3){            // Down -->  Scroll Down in the List
    if (CurrentFile == NumberofFiles){
      CurrentFile = 1;
    }
    else {
      CurrentFile++;
    }
    LCDBottomDelay = 1;
  }
  else if (ButtonPressed == 4){            // Left (Short) -->  Move back one directory
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
  else if (ButtonPressed == 94){           // Left (Long) -->  Move back to root directory
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
  else if (ButtonPressed == 5){            // Select (Short)-->  Change the selected drive
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
  else if (ButtonPressed == 95){           // Select (Long)-->  Disable/enable boot disk)
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
    LCDTopDelay = LCDDelay;
   }
  else if (ButtonPressed == 6){            // Swap -->  Swap the disks on Device 4
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
  }
}
