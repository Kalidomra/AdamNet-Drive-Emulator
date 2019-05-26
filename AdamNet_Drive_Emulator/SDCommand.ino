void SDCommand(byte devicenumber){                                 // Process a command for the SD card
  byte commandin = (long)WantedBlock >> 24;
  byte data1 = (long)WantedBlock >> 16;
  byte data2 = (long)WantedBlock >> 8;
  byte data3  = (long)WantedBlock;
  String CurrentFilename = "";
  Serial.print("SD Command: ");
  Serial.print(commandin,HEX);
  Serial.print(F(" : "));
  Serial.print(data1,HEX);
  Serial.print(F(" : "));
  Serial.print(data2,HEX);
  Serial.print(F(" : "));
  Serial.println(data3,HEX);
  if (commandin == 0xF1){                  // READ - Return the SD Card Current Directory
    // data1 = not used, data2 = length of filenames (max 60), data3 = Directory block to read
    // Directory will be sent in up to 255 blocks
    // Each directory block request should have the same filename length
    Serial.print(F("Adam Command: SD Card Directory Block: "));
    Serial.println(data3);
    if (data2>=60 || data2 ==0){
      data2=60;
    }
    Serial.print(F("Filename Length:"));
    Serial.println(data2);
    byte filesinblock = 1024/data2;
    Serial.print(F("Files in Block:"));
    Serial.println(filesinblock);
    BlockBuffer[devicenumber-4][0] = 0xB0 + devicenumber;  // Set up the header
    BlockBuffer[devicenumber-4][1] = 0x04;
    BlockBuffer[devicenumber-4][2] = 0x00;
    if ( data3 <= ((NumberofFiles/filesinblock) + 1 )){  // Are there still files to send.
      byte filenamebuffer[data2];
      for (int i=3; i<=1027;i++){          // Zero out the buffer, this make it easier to see when there is no more files.
        BlockBuffer[devicenumber-4][i] = 0x00;
      }
      int startingfilenumber = (filesinblock * (data3-1)) + 1;     
      for(int j = 0;j<= filesinblock-1;j++){  // loop through the files for the block
        CurrentFilename = GetFileName(FilesIndex[startingfilenumber+j]); // Get the name of the file from the sd card
        Serial.print(j+1);
        Serial.print(F(":"));
        Serial.println(CurrentFilename);
        for(int p=0;p<=(data2-1);p++){
          filenamebuffer[p] = 0x00;
        }
        CurrentFilename.getBytes(filenamebuffer, data2);  // Put the name in the buffer
        for(int i=0;i<=data2-1;i++){
          BlockBuffer[devicenumber-4][(j*data2)+i+3] = filenamebuffer[i]; // Fill the buffer with the file name
        }
        delay(100);
      }
    }
    else{                                  // No more files. Just send 0x00
      for (int i=3; i<=1026;i++){
        BlockBuffer[devicenumber-4][i] = 0x00;
      }
    }
    BlockBufferChecksum(devicenumber);       // Calculate the checksum before sending
  }
  if (commandin == 0xF2){                  // READ - Return Currently Mounted Disk
    // data1 = not used, data2 = not used , data3 = not used
    Serial.print(F("Adam Command: Current Disk on D"));
    Serial.println(devicenumber - 3);
    BlockBuffer[devicenumber-4][0] = 0xB0 + devicenumber; // Set up the header
    BlockBuffer[devicenumber-4][1] = 0x04;   // Tells Adam it's 1024 (0x400) bytes long
    BlockBuffer[devicenumber-4][2] = 0x00;
    BlockBuffer[devicenumber-4][3] = highByte(MountedFile[devicenumber-4]);
    BlockBuffer[devicenumber-4][4] = lowByte(MountedFile[devicenumber-4]);
    String mountedfilename = GetFileName(FilesIndex[MountedFile[devicenumber-4]]);
    byte mountedfilenamebuffer[mountedfilename.length()+1];
    mountedfilename.getBytes(mountedfilenamebuffer, mountedfilename.length()+1);
    for(int i=5;i<=(mountedfilename.length()+5);i++){  // Load the filename into the buffer
      BlockBuffer[devicenumber-4][i] = mountedfilenamebuffer[i-5];
    }
    for (int i = mountedfilename.length() + 6; i <= 1026 ; i++){ // Fill the rest of the buffer with 0x00
      BlockBuffer[devicenumber-4][i] = 0x00;
    }
    BlockBufferChecksum(devicenumber);         // Calculate the checksum before sending
  }
  if (commandin == 0xF3){                  // READ - Mount/Unmount Disk
    // data1 = not used, data2 = File Number (High) , data3 = File Number (Low)
    // The SD card directory starts at File Number 1 and increases by 1.
    // A File Number of 0 will unmount. 
    if (data2 == 0 && data3 ==0){
      MountedFile[devicenumber - 4] = 0;
      Serial.print(F("Adam Command: Unmount D"));
      Serial.println(devicenumber - 3);
      StatusSetup(0x43,devicenumber);      // Set the Status to "no disk"
    }
    else{
      MountedFile[devicenumber - 4] = word(data2,data3);
      Serial.print(F("Adam Command: Mount D"));
      Serial.print(devicenumber - 3);
      Serial.print(F(": "));
      Serial.println(GetFileName(FilesIndex[MountedFile[devicenumber-4]]));
      //SDLoadBlock(0, devicenumber);
      //LoadedBlock[devicenumber-4] = 0;
      LoadedBlock[devicenumber-4] = 0xFFFFFFFF;
      StatusSetup(0x40,devicenumber);      // Set the Status to 'disk in"
    }
    EepromStringWrite((devicenumber *300) + 2, GetFileName(FilesIndex[MountedFile[devicenumber-4]]));
    EEPROM.write(devicenumber *300, data2);
    EEPROM.write((devicenumber *300)+1, data3);
    refreshscreen = 1;
  }
  if (commandin == 0xF4){                  // WRITE - Write to LCD
    // data1 =  1 = Refresh, data2 = not used, data3 = 1 = Refresh
    if ((data3 == 1) || (data1 == 1)){
      Serial.println(F("Adam Command: Refresh LCD"));
      refreshscreen=1;
    }
    else{
      Serial.println(F("Adam Command: Writing to the LCD :"));
      lcd.clear();
      lcd.setCursor(0,0);
      for(int i =3; i<=18; i++){
        lcd.write(BlockBuffer[devicenumber-4][i]);
        Serial.write(BlockBuffer[devicenumber-4][i]);
      }
      Serial.println();
      lcd.setCursor(0,1);
      for(int i =19; i<=34; i++){
        lcd.write(BlockBuffer[devicenumber-4][i]);
        Serial.write(BlockBuffer[devicenumber-4][i]);
      }
      Serial.println();
      LCDScrollOn = false;                 // Turn off the LCD scroll until a key is pressed.
    }
  }
  if (commandin == 0xF5){                  // READ - Change Current SD Card Directory (Future)
  }
  if (commandin == 0xF6){                  // WRITE - Rename File (Future)
  }
  if (commandin == 0xF7){                  // READ - Delete File (Future)
  }
  if (commandin == 0xF8){                  // WRITE - Create File (Future)
  }
  if (commandin == 0xF9){                  // (Future)
  }
  if (commandin == 0xFA){                  // READ - Format Disk (Can be used if regular 0xFACE is disabled)
    // data1 = not used, data2 = not used , data3 = not used
    Serial.print(F("Adam Command: Format D"));
    Serial.println(devicenumber - 3);
    FACECommand(devicenumber);
  }
}
