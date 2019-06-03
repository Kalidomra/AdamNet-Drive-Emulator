void ProcessKeys(){                                                // LCD and Button Routines
  unsigned long ButtonDelay = millis() - LastButtonTime;
  byte RightButton;
  byte UpButton;
  byte DownButton;
  byte LeftButton;
  byte SelectButton;
  int keypressIn  = 1000;
  String LCDTemp = "";
  if (ButtonDelay >= ProcessKeysDelay){    // Only process the key inputs every so often.
    if (EnableAnalogButtons){
      keypressIn = analogRead(0);          // Read Analog pin 0 (Takes 110 us)
    }
    RightButton = digitalRead(RightButtonPin);
    UpButton = digitalRead(UpButtonPin);
    DownButton = digitalRead(DownButtonPin);
    LeftButton = digitalRead(LeftButtonPin);
    SelectButton = digitalRead(SelectButtonPin);
    if (keypressIn < AnalogTriggerRight || RightButton == LOW){   // Right -->  Mount or Unmount the Currently Selected Disk Image
      if (MountedFile[DeviceDisplayed-4] != 0){
        MountedFile[DeviceDisplayed-4] = 0;
        Serial.print(F("Unmounting D"));
        Serial.println(DeviceDisplayed - 3);
        StatusSetup(0x43,DeviceDisplayed); // Set the Status to "no disk"
      }
      else{
        MountedFile[DeviceDisplayed-4] = CurrentFile;
        Serial.print(F("Mounting D"));
        Serial.print(DeviceDisplayed - 3);
        Serial.print(F(": "));
        Serial.println(GetFileName(FilesIndex[MountedFile[DeviceDisplayed-4]]));
        LoadedBlock[DeviceDisplayed-4] = 0xFFFFFFFF; // Reset the loaded block for the new file
        StatusSetup(0x40,DeviceDisplayed); // Set the Status to 'disk in"
      }
      EepromStringWrite((DeviceDisplayed * 400) + 2, GetFileName(FilesIndex[MountedFile[DeviceDisplayed-4]]));
      byte hiByte = highByte(MountedFile[DeviceDisplayed-4]);
      byte loByte = lowByte(MountedFile[DeviceDisplayed-4]);
      EEPROM.write(DeviceDisplayed * 400, hiByte);
      EEPROM.write((DeviceDisplayed * 400) + 1, loByte);
      refreshscreen = 1;
    }
    else if (keypressIn < AnalogTriggerUp || UpButton == LOW){    // Up -->     Scroll Up in the List
      if (CurrentFile == 1){
        CurrentFile = NumberofFiles;
      }
      else {
        CurrentFile--;
      }
      refreshscreen = 1;
    }
    else if (keypressIn < AnalogTriggerDown || DownButton == LOW){// Down -->   Scroll Down in the List
      if (CurrentFile == NumberofFiles){
        CurrentFile = 1;
      }
      else {
        CurrentFile++;
      }
      refreshscreen = 1;
    }
    else if (keypressIn < AnalogTriggerLeft || LeftButton == LOW){// Left -->   Unmount the Disk Image
      MountedFile[DeviceDisplayed-4] = 0;
      Serial.print(F("Unmounting D"));
      Serial.println(DeviceDisplayed - 3);
      LoadedBlock[DeviceDisplayed-4] = 0xFFFFFFFF;// Reset the loaded block for the new file
      StatusSetup(0x43,DeviceDisplayed);   // Set the Status to "no disk"
      refreshscreen = 1;
    }  
    else if (keypressIn < AnalogTriggerSelect || SelectButton == LOW){// Select --> Change the Displayed Device Number
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
      refreshscreen = 1;
    }
    if (refreshscreen == 1){
      lcd.setCursor(0,0);
      lcd.print(F("D"));
      lcd.print(DeviceDisplayed - 3);
      lcd.print(F(":"));
      LCDTemp = GetFileName(FilesIndex[MountedFile[DeviceDisplayed-4]]).substring(0,13);
      byte LCDLength = LCDTemp.length();     
      for (int i = 0; i <= LCDLength; i++) {
        String topline = LCDTemp.substring(i,i+1);
        lcd.print(topline);
      }
      for (int i = LCDLength; i <= 12; i++) {
        lcd.print(" ");
      }
      LCDCurrentText = GetFileName(FilesIndex[CurrentFile]);
      lcd.setCursor(0,1);
      LCDLength = LCDCurrentText.length();
      if (LCDLength > 16){
        LCDLength = 16;
      }
      for (int i = 0; i < LCDLength; i++) {
        String bottomline = LCDCurrentText.substring(i,i+1);
        lcd.print(bottomline);
      }
      for (int i = LCDLength; i <= 16; i++) {
        lcd.print(" ");
      }
      CurrentLCDDelay = LCDScrollDelayStart;
      LCDScrollLocation = 0;
      LastScrollLCD = millis();
      refreshscreen = 0;
      LCDScrollOn = true;
    }
    LastButtonTime = millis();             // Reset the button press timer
  }
}