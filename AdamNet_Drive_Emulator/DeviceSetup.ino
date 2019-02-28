void DeviceSetup(){                                                // Initialize the eeprom and the devices
  byte numberofdrives = 0;
  Device4 = false;
  Device5 = false;
  Device6 = false;
  Device7 = false;
  if (EEPROM.read(0) == 65 && EEPROM.read(1) == 68 && EEPROM.read(2) == 69){    // Check if the eeprom has been initialized
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
    Serial.println(F(" Initializing Eeprom"));
    Device4= true;
    Device5= true;
    Device6= true;
    Device7= true;
    EEPROM.write(0,65);                    // A  )
    EEPROM.write(1,68);                    // D  } = Signature bytes to know that the eeprom has been configured.
    EEPROM.write(2,69);                    // E  )
    EEPROM.write(3,4);                     // Initial display device is 4
    EEPROM.write(4,1);                     // Enable Device 4
    EEPROM.write(5,1);                     // Enable Device 5
    EEPROM.write(6,1);                     // Enable Device 6
    EEPROM.write(7,1);                     // Enable Device 7
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
  devicenumber_displayed = EEPROM.read(3); // Load in the last device we were displaying
  if ((devicenumber_displayed == 4) && !Device4){ //Make sure it is still available, if not make it 0
    devicenumber_displayed = 0;
  }
  if ((devicenumber_displayed == 5) && !Device5){
    devicenumber_displayed = 0;
  }
  if ((devicenumber_displayed == 6) && !Device6){
    devicenumber_displayed = 0;
  }
  if ((devicenumber_displayed == 7) && !Device7){
    devicenumber_displayed = 0;
  }
  if ((devicenumber_displayed < 4) || (devicenumber_displayed > 7)){ // If the device is outside the correct numbers then re-assign to the lowest available device.
      if (Device4){
        devicenumber_displayed = 4; 
      }
      else if (Device5){
        devicenumber_displayed = 5; 
      }
      else if (Device6){
        devicenumber_displayed = 6; 
      }
      else if (Device7){
        devicenumber_displayed = 7; 
      }
  }
  if (Device4){
    if (bootdiskindex != 0){               // Do we have a boot disk to mount?
      MountedFile[0] = bootdiskindex;
      Serial.print(F("D1: <boot>: "));
      Serial.println(BootDisk);
      StatusSetup(0x40, 4);                // Set the status to "disk in"
      refreshscreen = 1;
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
}
void DeviceSetupCommand(byte devicenumber){
  String eepromfilename;
  byte hiByte =  EEPROM.read(devicenumber*300);
  byte lowByte =  EEPROM.read((devicenumber*300) + 1);
  MountedFile[devicenumber-4] =  word(hiByte, lowByte);
  eepromfilename = EepromStringRead((devicenumber *300) + 2);
  if (MountedFile[devicenumber-4] > maxfiles || eepromfilename != GetFileName(filesindex[MountedFile[devicenumber-4]])){
    MountedFile[devicenumber-4] = 0;
  }
  else if (MountedFile[devicenumber-4] != 0){
    Serial.print(F("D"));
    Serial.print(devicenumber - 3);
    Serial.print(F(": "));
    Serial.println(eepromfilename);
  }
  if (MountedFile[devicenumber-4] == 0){
    StatusSetup(0x43, devicenumber);       // Setup the Status array to "no disk"
    Serial.print(F("D"));
    Serial.print(devicenumber - 3);
    Serial.println(F(": < No Disk >"));
  }
  else{
    StatusSetup(0x40, devicenumber);       // Set the status to "disk in"
  }
}