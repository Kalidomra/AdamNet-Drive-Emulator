void ProcessKeys(){                                                // LCD and Button Routines
  unsigned long ButtonDelay = millis() - LastButtonTime;
  if (ButtonDelay >= 150){                 // Only process the key inputs every 150 ms
    keypressIn = analogRead(0);            // Read Analog pin 0 (Takes 110 us)
    if (IncomingCommandFlag == 0){         // Make sure we are not doing LCD stuff while Command is waiting
      if (keypressIn < 50){                // Right -->  Mount the Currently Selected Disk Image
            if (MountedFile[devicenumber_displayed-4] != 0){
              MountedFile[devicenumber_displayed-4] = 0;
              Serial.print("Unmounting D");
              Serial.println(devicenumber_displayed - 3);
              StatusSetup(0x43,devicenumber_displayed);                 // Set the status to "no disk"
            }
            else{
              MountedFile[devicenumber_displayed-4] = currentfile;
              Serial.print("Mounting D");
              Serial.print(devicenumber_displayed - 3);
              Serial.print(": ");
              Serial.println(GetFileName(filesindex[MountedFile[devicenumber_displayed-4]]));
              loadedblock[devicenumber_displayed-4] = -1;               // We don't have a block loaded for the new file
              StatusSetup(0x40,devicenumber_displayed);                 // Set the status to 'disk in"
            }
            EepromStringWrite((devicenumber_displayed * 100) + 2, GetFileName(filesindex[MountedFile[devicenumber_displayed-4]]));
            byte hiByte = highByte(MountedFile[devicenumber_displayed-4]);
            byte loByte = lowByte(MountedFile[devicenumber_displayed-4]);
            EEPROM.write(devicenumber_displayed*100, hiByte);
            EEPROM.write((devicenumber_displayed*100)+1, loByte);
            refreshscreen = 1;
      }  
      else if (keypressIn < 250){          // Up -->     Scroll Up in the List
        if (currentfile == 1){
          currentfile = numberoffiles;
        }
        else {
          currentfile--;
        }
        refreshscreen = 1;
      }
      else if (keypressIn < 450){          // Down -->   Scroll Down in the List
        if (currentfile == numberoffiles){
          currentfile = 1;
        }
        else {
          currentfile++;
        }
        refreshscreen = 1;
      }
      else if (keypressIn < 650){          // Left -->   Unmount the Disk Image
        MountedFile[devicenumber_displayed-4] = 0;
        Serial.print("Unmounting D");
        Serial.println(devicenumber_displayed - 3);
        loadedblock[devicenumber_displayed-4] = -1;// We don't have a block loaded for the new file
        StatusSetup(0x43,devicenumber_displayed); // Set the status to "no disk"
        refreshscreen = 1;
      }  
      else if (keypressIn < 850){          // Select --> Change the Displayed Device Number
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
        EEPROM.write(0,devicenumber_displayed);
        refreshscreen = 1;
      }
      if (refreshscreen == 1){
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("D");
        lcd.print(devicenumber_displayed - 3);
        lcd.print(":");
        lcd.print(GetFileName(filesindex[MountedFile[devicenumber_displayed-4]]).substring(0,37));
        LCDCurrentText = GetFileName(filesindex[currentfile]);
        lcd.setCursor(0,1);
        lcd.print(LCDCurrentText.substring(0,40));
        CurrentLCDDelay = LCDScrollDelayStart;
        LCDScrollLocation = 0;
        LastScrollLCD = millis();
        refreshscreen = 0;
      }
      LastButtonTime = millis();           // Reset the button press timer
    }
  }
}
