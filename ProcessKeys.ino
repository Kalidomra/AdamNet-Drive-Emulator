void ProcessKeys(){                                                // LCD and Button Routines
  unsigned long ButtonDelay = millis() - LastButtonTime;
  if (ButtonDelay >= 150){                 // Only process the key inputs every 150 ms
    keypressIn = analogRead(0);            // Read Analog pin 0 (Takes 110 us)
    if (IncomingCommandFlag == 0){         // Make sure we are not doing LCD stuff while Command is waiting
      if (keypressIn < 50){                // Right --> Mount the Currently Selected Disk Image
        MountedFile = currentfile;         // Make the mounted file the current file
        Serial.print("Mounting: ");
        Serial.println(sdfiles[MountedFile]);
        StatusSetup(0x40);                 // Set the status to 'disk in"
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
        MountedFile = 0;
        Serial.println("Unmounting Disk");
        StatusSetup(0x43);                 // Set the status to "no disk"
        loadedblock = -1;                  // No blocks loaded
        refreshscreen = 1;
      }  
      else if (keypressIn < 850){          // Select --> Change the Device Number
        if (devicenumber == 4){
          devicenumber = 5;
          EEPROM.write(0, 5);
        }
        else if (devicenumber == 5){
          devicenumber = 6;
          EEPROM.write(0, 6);
        }
        else if (devicenumber == 6){
          devicenumber = 7;
          EEPROM.write(0, 7);
        }
        else if (devicenumber == 7){
          devicenumber = 4;
          EEPROM.write(0, 4);
        }
        Serial.print("Changing to device: ");
        Serial.println(devicenumber);
        Serial.println("You need to reset Adam");
        StatusSetup(0x43);                 // Set the status to "no disk"
        loadedblock = -1;                  // No blocks loaded
        MountedFile = 0;
        refreshscreen = 1;
      }
      if (refreshscreen == 1){
        lcd.clear();
        lcd.setCursor(0,0);
        switch (devicenumber){
          case 4:
            lcd.print("D1:");
            break;
          case 5:
            lcd.print("D2:");
            break;
          case 6:
            lcd.print("D3:");
            break;
          case 7:
            lcd.print("D4:");
            break;
        }
        lcd.print(sdfiles[MountedFile]);
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