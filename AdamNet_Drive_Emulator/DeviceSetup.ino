void DeviceSetup(){                                                // Initialize the EEPROM and the devices
  byte numberofdrives = 0;
  boolean dirtest = false;
  boolean filetest = false;
  Device4 = false;
  Device5 = false;
  Device6 = false;
  Device7 = false;
  // Check if the EEPROM has been initialized. 3 Checks: 1 -> Bytes 'ADE', 2 -> LCDNameLength Hasn't Changed, 3 -> Version Hasn't Changed
  if ((EEPROM.read(0) == 65) && (EEPROM.read(1) == 68) && (EEPROM.read(2) == 69) &&
      (EEPROM.read(32) == LCDNameLength) && 
      (EEPROM.read(16) == Version[0]) && (EEPROM.read(17) == Version[1]) && (EEPROM.read(18) == Version[2])){ 
   if (EEPROM.read(4) == 1){
      Device4 = true;
      numberofdrives++;
    }
    if (EEPROM.read(5) == 1){
      Device5 = true;
      numberofdrives++;
    }
    if (EEPROM.read(6) == 1){
      Device6 = true;
      numberofdrives++;
    }
    if (EEPROM.read(7) == 1){
      Device7 = true;
      numberofdrives++;
    }
  }
  else{                                    // Eeprom is not initialized, let's set it up. Default is all drives on.
    Serial.println(F("EEPROM Not Initialized"));
    Serial.println(F("Erasing EEPROM:"));
    for (int w = 0; w <= 4096; w++){
      EEPROM.write(w,0x00);
      if ( w % 102 == 0){
        Serial.print(F("."));
      }
    }
    Serial.println();
    Serial.println(F("Initializing EEPROM"));
    Serial.println(F("D1, D2, D3 and D4 have been enabled"));
    Serial.println(F("Debug Mode has been disabled"));
    DebugMode = false;
    Device4= true;
    Device5= true;
    Device6= true;
    Device7= true;
    EEPROM.write(0,65);                    // A  )
    EEPROM.write(1,68);                    // D  } = Signature bytes to know that the EEPROM has been configured.
    EEPROM.write(2,69);                    // E  )
    EEPROM.write(3,4);                     // Initial display device is 4
    EEPROM.write(4,1);                     // Enable Device 4
    EEPROM.write(5,1);                     // Enable Device 5
    EEPROM.write(6,1);                     // Enable Device 6
    EEPROM.write(7,1);                     // Enable Device 7
    // Future Device bytes 8,9,10,11,12,13,14,15
    EEPROM.write(16,Version[0]);           //  )
    EEPROM.write(17,Version[1]);           //  } = Version Number
    EEPROM.write(18,Version[2]);           //  )
    EEPROM.write(32,LCDNameLength);        // The maximum file length to display on the LCD
    EEPROM.write(33,DebugMode);            // Enable for LCD debug mode
    numberofdrives = 4;
  }
  if (numberofdrives == 0){                // No Drives have been turned on
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(F("No Drives"));
    lcd.setCursor(0,1);
    lcd.print(F("Enabled"));
    while(1){}                             // Program will die here. Re-run config mode and turn on a drive.
  }
  DeviceDisplayed = EEPROM.read(3);        // Load in the last device that was displayed
  DebugMode = EEPROM.read(33);             // Load in Enable for LCD Debug Mode
  if ((DeviceDisplayed == 4) && !Device4){ //Make sure it is still available, if not make it 0
    DeviceDisplayed = 0;
  }
  if ((DeviceDisplayed == 5) && !Device5){
    DeviceDisplayed = 0;
  }
  if ((DeviceDisplayed == 6) && !Device6){
    DeviceDisplayed = 0;
  }
  if ((DeviceDisplayed == 7) && !Device7){
    DeviceDisplayed = 0;
  }
  if ((DeviceDisplayed < 4) || (DeviceDisplayed > 7)){ // If the device is outside the correct numbers then re-assign to the lowest available device.
      if (Device4){
        DeviceDisplayed = 4; 
      }
      else if (Device5){
        DeviceDisplayed = 5; 
      }
      else if (Device6){
        DeviceDisplayed = 6; 
      }
      else if (Device7){
        DeviceDisplayed = 7; 
      }    
  }
  if (Device4){
    if (BootDiskDir == "/"){
      dirtest = true;
    }
    else{
      dirtest = sd.chdir(BootDiskDir.c_str(),true);
    }
    filetest = sd.exists(BootDisk.c_str());
    if ((dirtest) && (filetest)){               // If there is a boot disk then mount it.
      file.open(BootDisk.c_str(), O_READ);
      BootDiskIndex = file.dirIndex();
      BootDiskExists = 1;
      BootDiskEnabled = 1;
      BootDiskMounted = 1;
      Serial.print(F("Boot Disk Dir: "));
      Serial.println(BootDiskDir);
      Serial.print(F("Boot Disk Index: "));
      Serial.println(BootDiskIndex);
      Serial.print(F("D1: <boot>: "));
      Serial.println(BootDisk);
      LoadedBlock[0] = 0xFFFFFFFF;
      StatusSetup(0x40, 4);                // Set the Status to "disk in"
      unsigned int BootDiskNumber = 0;     // Set the file number of the boot disk to 0. 
      EEPROM.put(1600, BootDiskNumber);
      EEPROM.put(1602, BootDiskIndex);
      EEPROM.put(1604, BootDiskType);
      EepromStringWrite(1605, BootDisk);
      EepromStringWrite(1705, BootDiskDir);
      LCDTopDelay = 1;
      LCDBottomDelay = 1;
      file.close();
    }
    else{
      DeviceSetupCommand(4);
    }
  }
  if (Device5){
    DeviceSetupCommand(5);
  }
  if (Device6){
    DeviceSetupCommand(6);
  }
  if (Device7){
    DeviceSetupCommand(7);
  }
  if (DebugMode){
    Serial.println(F("Debug Mode Enabled"));
  }
  else{
    Serial.println(F("Debug Mode Disabled"));
  }
  LCDTopDelay = 1;
  LCDBottomDelay = 1;
}
void DeviceSetupCommand(byte devicenumber){
  String eepromfilename;
  String eepromdir;
  eepromfilename = EepromStringRead((devicenumber * 400) + 5);
  eepromdir = EepromStringRead((devicenumber * 400) + 105);
  boolean dirtest = sd.chdir(eepromdir.c_str(),true);
  boolean filetest = sd.exists(eepromfilename.c_str());
  if (filetest && dirtest){ 
    Serial.print(F("D"));
    Serial.print(devicenumber - 3);
    Serial.print(F(": "));
    Serial.print(eepromdir);
    if (eepromdir != "/"){
      Serial.print(F("/"));
    }
    Serial.println(eepromfilename);
    StatusSetup(0x40, devicenumber);       // Set the Status to "disk in"
    LoadedBlock[devicenumber-4] = 0xFFFFFFFF;
  }
  else{
    StatusSetup(0x43,devicenumber);        // Set the Status to "no disk"
    EEPROM.write((devicenumber * 400), 0);                                                // Write the File Number
    EEPROM.write((devicenumber * 400) + 2, 0);                                            // Write the File Number Index
    EEPROM.write((devicenumber * 400) + 4, 0);                                            // Write the File Type = 0
    EepromStringWrite((devicenumber * 400) + 5, "");                                      // Write the File Name = "" 
    EepromStringWrite((devicenumber * 400) + 105, "");                                    // Write the current directory = ""
    Serial.print(F("D"));
    Serial.print(devicenumber - 3);
    Serial.println(F(": < No Disk >"));
  }

  BlockBuffer[devicenumber-4][0] = 0xB0 + devicenumber; // Need to zero out the buffer
  BlockBuffer[devicenumber-4][1] = 0x04;   
  BlockBuffer[devicenumber-4][2] = 0x00;
  for(int i=3;i<=1027;i++){ 
    BlockBuffer[devicenumber-4][i] = 0;
  }
}
