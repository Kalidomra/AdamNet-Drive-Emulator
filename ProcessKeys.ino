void ProcessKeys(){                                                // LCD and Button Routines
  unsigned long ButtonDelay = millis() - LastButtonTime;
  if (ButtonDelay >= 150){                 // Only process the key inputs every 150 ms
    keypressIn = analogRead(0);            // Read Analog pin 0 (Takes 110 us)
    if (IncomingCommandFlag == 0){         // Make sure we are not doing LCD stuff while Command is waiting
      if (keypressIn < 50){                // Right --> Mount the Currently Selected Disk Image
        switch (devicenumber_displayed){
          case 4:
            if (MountedFile4 != 0){
              MountedFile4 = 0;
              Serial.println("Unmounting D1");
              StatusSetup(0x43,devicenumber_displayed);                 // Set the status to "no disk"
            }
            else{
              MountedFile4 = currentfile;
              Serial.print("Mounting on D1: ");
              Serial.println(sdfiles[MountedFile4]);
              StatusSetup(0x40,devicenumber_displayed);                 // Set the status to 'disk in"
            }
            EepromStringWrite(400, sdfiles[MountedFile4]);
            EEPROM.write(4, MountedFile4);
            break;
          case 5:
            if (MountedFile5 != 0){
              MountedFile5 = 0;
              Serial.println("Unmounting D2");
              StatusSetup(0x43,devicenumber_displayed);                 // Set the status to "no disk"
            }
            else{
              MountedFile5 = currentfile;
              Serial.print("Mounting on D2: ");
              Serial.println(sdfiles[MountedFile5]);
              StatusSetup(0x40,devicenumber_displayed);                 // Set the status to 'disk in"
            }
            EepromStringWrite(500, sdfiles[MountedFile5]);            
            EEPROM.write(5, MountedFile5);
            break;
          case 6:            
            if (MountedFile6 != 0){
              MountedFile6 = 0;
              Serial.println("Unmounting D3");
              StatusSetup(0x43,devicenumber_displayed);                 // Set the status to "no disk"
            }
            else{
              MountedFile6 = currentfile;
              Serial.print("Mounting on D3: ");
              Serial.println(sdfiles[MountedFile6]);
              StatusSetup(0x40,devicenumber_displayed);                 // Set the status to 'disk in"
            }
            EepromStringWrite(600, sdfiles[MountedFile6]);            
            EEPROM.write(6, MountedFile6);
            break;
          case 7:           
            if (MountedFile7 != 0){
              MountedFile7 = 0;
              Serial.println("Unmounting D4");
              StatusSetup(0x43,devicenumber_displayed);                 // Set the status to "no disk"
            }
            else{
              MountedFile7 = currentfile;
              Serial.print("Mounting on D4: ");
              Serial.println(sdfiles[MountedFile7]);
              StatusSetup(0x40,devicenumber_displayed);                 // Set the status to 'disk in"
            }
            EepromStringWrite(700, sdfiles[MountedFile7]);            
            EEPROM.write(7, MountedFile7);
            break;
        }        
        loadedblock = -1;                  // We don't have a block loaded for the new file
        refreshscreen = 1;
      }  
      else if (keypressIn < 250){          // Up --> Scroll Up in the List
        if (currentfile == 1){
          currentfile = numberoffiles;
        }
        else {
          currentfile--;
        }
        refreshscreen = 1;
      }
      else if (keypressIn < 450){          // Down --> Scroll Down in the List
        if (currentfile == numberoffiles){
          currentfile = 1;
        }
        else {
          currentfile++;
        }
        refreshscreen = 1;
      }
      else if (keypressIn < 650){          // Left --> Unmount the Disk Image
        //MountedFile = 0;

        switch (devicenumber_displayed){
          case 4:
            MountedFile4 = 0;
            Serial.println("Unmounting D1");
            break;
          case 5:
            MountedFile5 = 0;
            Serial.println("Unmounting D2");
            break;
          case 6:
            MountedFile6 = 0;
            Serial.println("Unmounting D3");
            break;
          case 7:
            MountedFile7 = 0;
            Serial.println("Unmounting D4");
            break;            
        }
        StatusSetup(0x43,devicenumber_displayed);                 // Set the status to "no disk"
        loadedblock = -1;                  // No blocks loaded
        refreshscreen = 1;
      }  
      else if (keypressIn < 850){          // Select --> Change the Displayed Device Number
        if (devicenumber_displayed < NumberofDrives + 3){
          devicenumber_displayed ++;
        }
        else {
          devicenumber_displayed = 4;
        }
        EEPROM.write(0,devicenumber_displayed);
        refreshscreen = 1;
      }
      if (refreshscreen == 1){
        lcd.clear();
        lcd.setCursor(0,0);
        switch (devicenumber_displayed){
          case 4:
            lcd.print("D1:");
            lcd.print(sdfiles[MountedFile4]);
            break;
          case 5:
            lcd.print("D2:");
            lcd.print(sdfiles[MountedFile5]);
            break;
          case 6:
            lcd.print("D3:");
            lcd.print(sdfiles[MountedFile6]);
            break;
          case 7:
            lcd.print("D4:");
            lcd.print(sdfiles[MountedFile7]);
            break;
        }
        lcd.setCursor(0,1);
        lcd.print(sdfiles[currentfile]);
        CurrentLCDDelay = LCDScrollDelayStart;
        LCDScrollLocation = 0;
        LastScrollLCD = millis();
        refreshscreen = 0;
      }
      LastButtonTime = millis();           // Reset the button press timer
    }
  }
}
