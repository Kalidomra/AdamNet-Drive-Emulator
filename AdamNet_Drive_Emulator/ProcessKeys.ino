void ProcessKeys(){                                                // LCD and Button Routines
  unsigned long ButtonDelay = millis() - LastButtonTime;
  byte RightButton;
  byte UpButton;
  byte DownButton;
  byte LeftButton;
  byte SelectButton;
  if (ButtonDelay >= 150){                 // Only process the key inputs every 150 ms
    if (EnableAnalogButtons){
      keypressIn = analogRead(0);          // Read Analog pin 0 (Takes 110 us)
    }
    else {
      keypressIn = 5000;
    }
    if (IncomingCommandFlag == 0){         // Make sure we are not doing LCD stuff while Command is waiting
      RightButton = digitalRead(RightButtonPin);
      if (IncomingCommandFlag == 1){return;} 
      UpButton = digitalRead(UpButtonPin);
      if (IncomingCommandFlag == 1){return;}
      DownButton = digitalRead(DownButtonPin);
      if (IncomingCommandFlag == 1){return;}
      LeftButton = digitalRead(LeftButtonPin);
      if (IncomingCommandFlag == 1){return;}
      SelectButton = digitalRead(SelectButtonPin);
      if (IncomingCommandFlag == 1){return;}
      if (keypressIn < 50 || RightButton == LOW){       // Right -->  Mount or Unmount the Currently Selected Disk Image
            if (MountedFile[devicenumber_displayed-4] != 0){
              MountedFile[devicenumber_displayed-4] = 0;
              Serial.print(F("Unmounting D"));
              Serial.println(devicenumber_displayed - 3);
              StatusSetup(0x43,devicenumber_displayed); // Set the status to "no disk"
            }
            else{
              MountedFile[devicenumber_displayed-4] = currentfile;
              Serial.print(F("Mounting D"));
              Serial.print(devicenumber_displayed - 3);
              Serial.print(F(": "));
              Serial.println(GetFileName(filesindex[MountedFile[devicenumber_displayed-4]]));
              loadedblock[devicenumber_displayed-4] = 0xFFFFFFFF; // We don't have a block loaded for the new file
              StatusSetup(0x40,devicenumber_displayed); // Set the status to 'disk in"
            }
            EepromStringWrite((devicenumber_displayed * 300) + 2, GetFileName(filesindex[MountedFile[devicenumber_displayed-4]]));
            byte hiByte = highByte(MountedFile[devicenumber_displayed-4]);
            byte loByte = lowByte(MountedFile[devicenumber_displayed-4]);
            EEPROM.write(devicenumber_displayed*300, hiByte);
            EEPROM.write((devicenumber_displayed*300)+1, loByte);
            refreshscreen = 1;
      }  
      else if (keypressIn < 250 || UpButton == LOW){    // Up -->     Scroll Up in the List
        if (currentfile == 1){
          currentfile = numberoffiles;
        }
        else {
          currentfile--;
        }
        refreshscreen = 1;
      }
      else if (keypressIn < 450 || DownButton == LOW){  // Down -->   Scroll Down in the List
        if (currentfile == numberoffiles){
          currentfile = 1;
        }
        else {
          currentfile++;
        }
        refreshscreen = 1;
      }
      else if (keypressIn < 650 || LeftButton == LOW){  // Left -->   Unmount the Disk Image
        MountedFile[devicenumber_displayed-4] = 0;
        Serial.print(F("Unmounting D"));
        Serial.println(devicenumber_displayed - 3);
        loadedblock[devicenumber_displayed-4] = 0xFFFFFFFF;// We don't have a block loaded for the new file
        StatusSetup(0x43,devicenumber_displayed); // Set the status to "no disk"
        refreshscreen = 1;
      }  
      else if (keypressIn < 850 || SelectButton == LOW){// Select --> Change the Displayed Device Number
        switch(devicenumber_displayed){
          case 4:
            if(Device5){
              devicenumber_displayed = 5;
            }
            else if(Device6){
              devicenumber_displayed = 6;
            }
            else if(Device7){
              devicenumber_displayed = 7;
            }
            else {
              devicenumber_displayed = 4;
            }
            break;
          case 5:
            if(Device6){
              devicenumber_displayed = 6;
            }
            else if(Device7){
              devicenumber_displayed = 7;
            }
            else if(Device4){
              devicenumber_displayed = 4;
            }
            else {
              devicenumber_displayed = 5;
            }
            break;
          case 6:
            if(Device7){
              devicenumber_displayed = 7;
            }
            else if(Device4){
              devicenumber_displayed = 4;
            }
            else if(Device5){
              devicenumber_displayed = 5;
            }
            else {
              devicenumber_displayed = 6;
            }
            break;
          case 7:
            if(Device4){
              devicenumber_displayed = 4;
            }
            else if(Device5){
              devicenumber_displayed = 5;
            }
            else if(Device6){
              devicenumber_displayed = 6;
            }
            else {
              devicenumber_displayed = 7;
            }
            break;
        }
        EEPROM.write(3,devicenumber_displayed);
        refreshscreen = 1;
      }
      if (refreshscreen == 1 && IncomingCommandFlag != 1){
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print(F("D"));
        lcd.print(devicenumber_displayed - 3);
        lcd.print(F(":"));
        lcd.print(GetFileName(filesindex[MountedFile[devicenumber_displayed-4]]).substring(0,37));
        LCDCurrentText = GetFileName(filesindex[currentfile]);
        lcd.setCursor(0,1);
        lcd.print(LCDCurrentText.substring(0,40));
        CurrentLCDDelay = LCDScrollDelayStart;
        LCDScrollLocation = 0;
        LastScrollLCD = millis();
        refreshscreen = 0;
        LCDScrollOn = true;
      }
      LastButtonTime = millis();           // Reset the button press timer
    }
  }
}