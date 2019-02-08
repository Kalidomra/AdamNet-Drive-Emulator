void DeviceSetup(){
  devicenumber_displayed = EEPROM.read(0);  // Load in the last device we were displaying
  if ((devicenumber_displayed == 4) && !Device4){//Make sure it is still available, if not make it 0
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
  if ((devicenumber_displayed < 4) || (devicenumber_displayed > 7)){// If the device is outside the correct numbers then re-assign to the lowest available device.
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
      Serial.print("D1: <boot>: ");
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
    byte hiByte =  EEPROM.read(devicenumber*100);
    byte lowByte =  EEPROM.read((devicenumber*100) + 1);
    MountedFile[devicenumber-4] =  word(hiByte, lowByte);
    eepromfilename = EepromStringRead((devicenumber *100) + 2);
    if (MountedFile[devicenumber-4] > maxfiles || eepromfilename != GetFileName(filesindex[MountedFile[devicenumber-4]])){
      MountedFile[devicenumber-4] = 0;
    }
    else if (MountedFile[devicenumber-4] != 0){
      Serial.print("D");
      Serial.print(devicenumber - 3);
      Serial.print(": ");
      Serial.println(eepromfilename);
    }
    if (MountedFile[devicenumber-4] == 0){
      StatusSetup(0x43, devicenumber);                     // Setup the Status array to "no disk"
      Serial.print("D");
      Serial.print(devicenumber - 3);
      Serial.println(": < No Disk >");
    }
    else{
      StatusSetup(0x40, devicenumber);                    // Set the status to "disk in"
    }
  }
