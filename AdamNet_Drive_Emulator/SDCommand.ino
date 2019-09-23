void SDCommand(byte devicenumber){                                 // Process a command for the SD card
  byte commandin = (long)WantedBlock >> 24;
  byte data1 = (long)WantedBlock >> 16;
  byte data2 = (long)WantedBlock >> 8;
  byte data3  = (long)WantedBlock;
  String CurrentFilename = "";
  Serial.print(F("SD Command: "));
  Serial.print(commandin,HEX);
  Serial.print(F(" : "));
  Serial.print(data1,HEX);
  Serial.print(F(" : "));
  Serial.print(data2,HEX);
  Serial.print(F(" : "));
  Serial.println(data3,HEX);
  if (DebugMode){
    lcd.clear();
    lcd.setCursor(0,1);
    lcd.print(F("SD="));
    lcd.print(commandin,HEX);
    lcd.print(F(":"));
    lcd.print(data1,HEX);
    lcd.print(F(":"));
    lcd.print(data2,HEX);
    lcd.print(F(":"));
    lcd.print(data3,HEX);
    LCDBottomDelay = LCDDelay;
  }
  if (commandin == 0xF1){                  // READ - Return the SD Card Current Directory
    // data1 = 0 -> Do not send filetypes
    //       = 1 -> Send the filetype as the first byte of the file name 
    //              (0=[/], 1=[..], 2=DIR, 10=DSK, 11=DDP, 22=DS2, 23=DS3, 24=DS4
    // data2 = length of filenames (max 60)
    // data3 = Directory block to read
    // Directory will be sent in up to 255 blocks
    // Each directory block request should have the same filename length

    Serial.print(F("Adam Command: SD Card Directory Block: "));
    Serial.println(data3);
    if (data2>=60 || data2 ==0){
      data2 = 60;
    }
    Serial.print(F("Filename Length:"));
    Serial.println(data2);
    byte filesinblock = 1024/data2;
    Serial.print(F("Files in Block:"));
    Serial.println(filesinblock);
    Serial.print(F("Current Directory:"));
    Serial.println(CurrentDirectory);
    Serial.print(F("Number of Files:"));
    Serial.println(NumberofFiles);
    sd.chdir(CurrentDirectory.c_str(),true);
    BlockBuffer[devicenumber-4][0] = 0xB0 + devicenumber;  // Set up the header
    BlockBuffer[devicenumber-4][1] = 0x04;
    BlockBuffer[devicenumber-4][2] = 0x00;
    float maxblock= ceil((float)NumberofFiles/(float)filesinblock);
    if ( data3  <= maxblock){              // Are there still files to send.
      byte filenamebuffer[data2];
      for (int i=3; i<=1027;i++){          // Zero out the buffer, this make it easier to see when there is no more files.
        BlockBuffer[devicenumber-4][i] = 0x00;
      }
      int startingfilenumber = (filesinblock * (data3-1)) + 1;
      for(int j = 0;j<= filesinblock-1;j++){ // loop through the files for the block     
        if (data1 == 1){
          CurrentFilename = GetFileName(startingfilenumber+j, data2-1); // Get the name of the file from the sd card
          CurrentFilename = TypeIndex[startingfilenumber+j] + CurrentFilename;  // If data1 is 1, then we add the file type to filename.
        }
        else{
          CurrentFilename = GetFileName(startingfilenumber+j, data2); // Get the name of the file from the sd card
        }
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
    BlockBufferChecksum(devicenumber);     // Calculate the checksum before sending
  }
  if (commandin == 0xF2){                  // READ - Return Currently Mounted Disk
    // data1 = not used, data2 = not used , data3 = not used
    Serial.print(F("Adam Command: Current Disk on D"));
    Serial.println(devicenumber - 3);
    BlockBuffer[devicenumber-4][0] = 0xB0 + devicenumber; // Set up the header
    BlockBuffer[devicenumber-4][1] = 0x04;   // Tells Adam it's 1024 (0x400) bytes long
    BlockBuffer[devicenumber-4][2] = 0x00;
    BlockBuffer[devicenumber-4][3] = EEPROM.read(devicenumber * 400);
    BlockBuffer[devicenumber-4][4] = EEPROM.read((devicenumber * 400) + 1);
    String mountedfilename;
    if (BootDiskMounted == 1){
      mountedfilename = "/" + BootDisk;
    }
    else{
      mountedfilename = EepromStringRead((devicenumber * 400) + 105);
      if (mountedfilename == "/"){
        mountedfilename = "/" + EepromStringRead((devicenumber * 400) + 5);
      }
      else{
        mountedfilename = mountedfilename + "/" + EepromStringRead((devicenumber * 400) + 5);
      }
    }
    byte mountedfilenamebuffer[mountedfilename.length()+1];
    mountedfilename.getBytes(mountedfilenamebuffer, mountedfilename.length()+1);
    for(int i=5;i<=(mountedfilename.length()+5);i++){  // Load the filename into the buffer
      BlockBuffer[devicenumber-4][i] = mountedfilenamebuffer[i-5];
    }
    for (int i = mountedfilename.length() + 6; i <= 1026 ; i++){ // Fill the rest of the buffer with 0x00
      BlockBuffer[devicenumber-4][i] = 0x00;
    }
    BlockBufferChecksum(devicenumber);     // Calculate the checksum before sending
  }
  if (commandin == 0xF3){                  // READ - Mount/Unmount Disk
    // data1 = 1 = Disable Next Reset, data2 = File Number (High) , data3 = File Number (Low)
    // The SD card directory starts at File Number 1 and increases by 1.
    // This read returns the type of file mounted as the first byte. 0 = Root Dir, 1 = Previous Dir, 2 = Dir, 10 = Disk, 11 = DDP
    // A File Number of 0 will unmount. 
    if (data1 == 1){
     DisableNextReset = true;
    }
    sd.chdir(CurrentDirectory.c_str(),true);
    unsigned int FiletoMount = word(data2,data3);
    unsigned int IndextoSend = TypeIndex[FiletoMount];
    if (data2 == 0 && data3 ==0){          // Unmount
      Serial.print(F("Unmounting D"));
      Serial.println(devicenumber - 3);
      StatusSetup(0x43,devicenumber); // Set the Status to "no disk"
      EEPROM.write((devicenumber * 400), 0); // Write the File Number
      EEPROM.write((devicenumber * 400) + 2, 0); // Write the File Number Index
      EEPROM.write((devicenumber * 400) + 4, 0); // Write the File Type = 0
      EepromStringWrite((devicenumber * 400) + 5, "");
      EepromStringWrite((devicenumber * 400) + 105, "");
      BootDiskMounted = 0;
    }
    else if ((TypeIndex[FiletoMount] == 10) || (TypeIndex[FiletoMount] == 11) || (TypeIndex[FiletoMount] == 12)
             || (TypeIndex[FiletoMount] == 22) || (TypeIndex[FiletoMount] == 23) || (TypeIndex[FiletoMount] == 24)){   // This is a DSK, DDP, ROM, DS2, DS3 or DS4 file
      String FileNametoMount = GetFileName(FiletoMount,99);
      Serial.print(F("Mounting D"));       // Mount
      Serial.print(devicenumber - 3);
      Serial.print(F(": "));
      Serial.println(FileNametoMount);
      LoadedBlock[devicenumber-4] = 0xFFFFFFFF; // Reset the loaded block for the new file
      StatusSetup(0x40,devicenumber); // Set the Status to 'disk in"
      EEPROM.put((devicenumber * 400), FiletoMount);                    // Write the File Number
      EEPROM.put((devicenumber * 400) + 2, FilesIndex[FiletoMount]);    // Write the File Number Index
      EEPROM.put((devicenumber * 400) + 4, TypeIndex[FiletoMount]);     // Write the File Type
      EepromStringWrite((devicenumber * 400) + 5, FileNametoMount);
      EepromStringWrite((devicenumber * 400) + 105, CurrentDirectory);
      BootDiskMounted = 0;
    }
    else if (TypeIndex[FiletoMount] == 1){ // This is the previous directory [..]
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
      SDCardGetDir();
      SetCurrentFile();
    }
    else if (TypeIndex[FiletoMount] == 0){ // This is a go back to root. [/]
      CurrentDirectory = "/";
      Serial.print(F("Changing to Directory: "));
      Serial.println(CurrentDirectory);
      sd.chdir(CurrentDirectory.c_str(),true);
      SDCardGetDir();
      SetCurrentFile();
    }
    else if (TypeIndex[FiletoMount] == 2){ // This is a directory
      String tempdir = GetFileName(FiletoMount,99);
      if (CurrentDirectory == "/"){
        CurrentDirectory = CurrentDirectory + tempdir.substring(1, tempdir.length()-1);
      }
      else{
        CurrentDirectory = CurrentDirectory + "/" + tempdir.substring(1, tempdir.length()-1);
      }
      Serial.print(F("Changing to Directory: "));
      Serial.println(CurrentDirectory);
      sd.chdir(CurrentDirectory.c_str(),true);
      SDCardGetDir();
      SetCurrentFile();
    }
    BlockBuffer[devicenumber-4][0] = 0xB0 + devicenumber; // Set up the header
    BlockBuffer[devicenumber-4][1] = 0x04;   // Tells Adam it's 1024 (0x400) bytes long
    BlockBuffer[devicenumber-4][2] = 0x00;
    BlockBuffer[devicenumber-4][3] = IndextoSend;
    for (int i = 4; i <= 1026 ; i++){      // Fill the rest of the buffer with 0x00
      BlockBuffer[devicenumber-4][i] = 0x00;
    }
    BlockBufferChecksum(devicenumber);     // Calculate the checksum before sending
    LCDTopDelay = 1;
  }
  if (commandin == 0xF4){                  // WRITE - Write to LCD
    // data1 =  1 = Refresh, data2 = not used, data3 = 1 = Refresh
    if ((data3 == 1) || (data1 == 1)){
      Serial.println(F("Adam Command: Refresh LCD"));
      LCDTopDelay = 1;
      LCDBottomDelay = 1;
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
  if (commandin == 0xF5){                  // Write - Directory Commands (Change to, Create, Delete, Rename)  (Future)
  }
  if (commandin == 0xF6){                  // Write - File Commands (Create, Delete, Rename, Copy, Move) (Future)
  }
  if (commandin == 0xF7){                  // (Future)
  }
  if (commandin == 0xF8){                  // (Future)
  }
  if (commandin == 0xF9){                  // (Future)
  }
  if (commandin == 0xFA){                  // READ - Format Disk (Can be used if regular 0xFACE is disabled)
    // data1 = not used, must be 0, data2 = 0 on first read, 0xFA on second, data3 = not used, must be 0
    // This format command uses 2 reads in order to initiate the format.
    // First a read to 0xFA000000 and then a read to 0xFA00FA00 to confirm the format.
    // They need to be in order with no block reads between them.
    if( data1 == 0 && data2 == 0 && data3 == 0 && SDCommandFAConfirm == 0){  // Received the format command, need confirmation
      Serial.print(F("Adam Command: Format D"));
      Serial.println(devicenumber - 3);
      SDCommandFAConfirm = 2;
    }
    else if ( data1 == 0 && data2 == 0xFA && data3 == 0 && SDCommandFAConfirm == 1){  // Received a confirmation as the next read
      Serial.println(F("Format Command Confirmed"));
      FACECommand(devicenumber);
    }
    else {
      Serial.println(F("Format Command Error"));
    }
  }
}
