void SDCommand(byte devicenumber){                                 // Process a command for the SD card
  byte commandin = (long)WantedBlock >> 24;
  byte data1 = (long)WantedBlock >> 16;
  byte data2 = (long)WantedBlock >> 8;
  byte data3  = (long)WantedBlock;
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
  if (commandin == 0xF1){                       // READ - Return the SD Card Current Directory
    CurrentDirectoryReturn(devicenumber);
  }
  else if (commandin == 0xF2){                  // READ - Return Currently Mounted Disk
    CurrentlyMounted(devicenumber);
  }
  else if (commandin == 0xF3){                  // READ - Mount/Unmount Disk
    MountUnmount(devicenumber);
  }
  else if (commandin == 0xF4){                  // WRITE - Write to LCD
     WritetoLCD(devicenumber);
  }
  else if (commandin == 0xF5){                  // WRITE - Save ADE Config and reboot ADE
    SaveADEConfig(devicenumber);
  }
  else if (commandin == 0xF6){                  // READ - Delete File or Directory
    Delete(devicenumber);
  }
  else if (commandin == 0xF7){                  // WRITE - Create Image File or Directory
    NewImage(devicenumber);
  }
  else if (commandin == 0xF8){                  // WRITE - Rename Image File or Directory
    Rename(devicenumber);
  }
  else if (commandin == 0xF9){                  // READ - Cut, Copy, Paste
    CutCopyPaste(devicenumber);
  }
  else if (commandin == 0xFA){                  // READ - Format Disk (Can be used if regular 0xFACE is disabled)
    Format(devicenumber);
  }
}
void  CurrentDirectoryReturn(byte devicenumber){         // Command 0xF1
  // data1 = 0 -> Do not send filetypes
  //       = 1 -> Send the filetype as the first byte of the file name 
  //              (0=[/], 1=[..], 2=DIR, 10=DSK, 11=DDP, 22=DS2, 23=DS3, 24=DS4
  // data2 = length of filenames (max 60)
  // data3 = Directory block to read
  // Directory will be sent in up to 255 blocks
  // Each directory block request should have the same filename length
  byte data1 = (long)WantedBlock >> 16;
  byte data2 = (long)WantedBlock >> 8;
  byte data3  = (long)WantedBlock;
  String CurrentFilename = "";
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
      if ((startingfilenumber+j) <= MaxFiles){
        if (data1 == 1){
          char typechar[1];
          typechar[0] = char(TypeIndex[startingfilenumber+j]);
          CurrentFilename = typechar[0];
          CurrentFilename = CurrentFilename + GetFileName(startingfilenumber+j, data2-1); // Get the name of the file from the sd card
          Serial.print(j+1);
          Serial.print(F(":"));
          Serial.println(CurrentFilename.substring(1));
        }
        else{
          CurrentFilename = GetFileName(startingfilenumber+j, data2); // Get the name of the file from the sd card
          Serial.print(j+1);
          Serial.print(F(":"));
          Serial.println(CurrentFilename);
        }
      }
      else{
          CurrentFilename = "";
          Serial.print(j+1);
          Serial.print(F(":"));
          Serial.println(CurrentFilename);
      }
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
  if (TooMany == 1){
      StatusSetup(0x41, devicenumber);
  }
  else{
    StatusSetup(0x40, devicenumber);
  }
}
void  CurrentlyMounted(byte devicenumber){     // Command 0xF2
  // data1 = not used, data2 = not used , data3 = not used
  Serial.print(F("Adam Command: Current Disk on D"));
  Serial.println(devicenumber - 3);
  BlockBuffer[devicenumber-4][0] = 0xB0 + devicenumber; // Set up the header
  BlockBuffer[devicenumber-4][1] = 0x04;   // Tells Adam it's 1024 (0x400) bytes long
  BlockBuffer[devicenumber-4][2] = 0x00;
  BlockBuffer[devicenumber-4][3] = EEPROM.read(devicenumber * 400);
  BlockBuffer[devicenumber-4][4] = EEPROM.read((devicenumber * 400) + 1);
  String mountedfilename;
  if (BootDiskMounted == 1 && devicenumber == 4){
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
void  MountUnmount(byte devicenumber){         // Command 0xF3
  // data1 = 1 = Disable Next Reset, data2 = File Number (High) , data3 = File Number (Low)
  // The SD card directory starts at File Number 1 and increases by 1.
  // This read returns the type of file mounted as the first byte. 0 = Root Dir, 1 = Previous Dir, 2 = Dir, 10 = Disk, 11 = DDP
  // A File Number of 0 will unmount. 
  byte data1 = (long)WantedBlock >> 16;
  byte data2 = (long)WantedBlock >> 8;
  byte data3  = (long)WantedBlock;
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
    if (devicenumber == 4){
      BootDiskMounted = 0;
    }
   // LCDTopDelay = 1;
   // LCDBottomDelay = 1;
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
    if (devicenumber == 4){
      BootDiskMounted = 0;
    }
  }
  else if (TypeIndex[FiletoMount] == 2){ // This is the previous directory [..]
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
    SDCardGetDir(0);
    SetCurrentFile();
  }
  else if (TypeIndex[FiletoMount] == 1){ // This is a go back to root. [/]
    CurrentDirectory = "/";
    Serial.print(F("Changing to Directory: "));
    Serial.println(CurrentDirectory);
    sd.chdir(CurrentDirectory.c_str(),true);
    SDCardGetDir(0);
    SetCurrentFile();
  }
  else if (TypeIndex[FiletoMount] == 3){ // This is a directory
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
    SDCardGetDir(0);
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
}
void  WritetoLCD(byte devicenumber){           // Command 0xF4
  // data1 =  1 = Refresh, data2 = not used, data3 = 1 = Refresh
  byte data1 = (long)WantedBlock >> 16;
  byte data2 = (long)WantedBlock >> 8;
  byte data3  = (long)WantedBlock;
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
void  SaveADEConfig(byte devicenumber){        // Command 0xF5
  // data1 =  not used, data2 = not used, data3 = not used
  // Block Buffer Byte 7 = Device 4 enable
  // Block Buffer Byte 8 = Device 5 enable
  // Block Buffer Byte 9 = Device 6 enable
  // Block Buffer Byte 10 = Device 7 enable
  // Block Buffer Byte 17 = Debug Mode Enable
  interrupts();                          // Enable Interrupts
  Serial.println(F("Adam Command: Writing ADE Config"));
  EEPROM.write(4,BlockBuffer[devicenumber-4][7]);
  EEPROM.write(5,BlockBuffer[devicenumber-4][8]);
  EEPROM.write(6,BlockBuffer[devicenumber-4][9]);
  EEPROM.write(7,BlockBuffer[devicenumber-4][10]);
  EEPROM.write(33,BlockBuffer[devicenumber-4][17]);
  Serial.print(F("Device 4: "));
  Serial.println(BlockBuffer[devicenumber-4][7]);
  Serial.print(F("Device 5: "));
  Serial.println(BlockBuffer[devicenumber-4][8]);
  Serial.print(F("Device 6: "));
  Serial.println(BlockBuffer[devicenumber-4][9]);
  Serial.print(F("Device 7: "));
  Serial.println(BlockBuffer[devicenumber-4][10]);
  Serial.print(F("Debug Mode: "));
  Serial.println(BlockBuffer[devicenumber-4][17]);
  Serial.println(F("Rebooting..."));
  delay(1000);
  void(* resetFunc) (void) = 0;
  resetFunc(); 
}
void  Delete(byte devicenumber){               // Command 0xF6
  // data1 = 0 on the first read, F6 on the second, data2 = File Number (High) , data3 = File Number (Low)
  // This delete command uses 2 reads in order to initiate the image delete. Both include the file number
  // First a read to High Word 0xF600 and then a read to High Word 0xF6F6 to confirm the delete.
  // They need to be in order with no block reads between them.
  interrupts();                          // Enable Interrupts
  byte data1 = (long)WantedBlock >> 16;
  byte data2 = (long)WantedBlock >> 8;
  byte data3  = (long)WantedBlock;
  if( data1 == 0 && SDCommandF6Confirm == 0){  // Received the delete command, need confirmation
    Serial.println(F("Adam Command: Delete Image File"));
    SDCommandF6Confirm = 2;
  }
  else if ( data1 == 0xF6 && SDCommandF6Confirm == 1){  // Received a confirmation as the next read
    Serial.println(F("Delete Command Confirmed"));
    sd.chdir(CurrentDirectory.c_str(),true);
    unsigned int FiletoDelete = word(data2,data3);
    unsigned int IndextoDelete = FilesIndex[FiletoDelete];
    String FileNametoDelete = GetFileName(FiletoDelete,99);
    if (TypeIndex[FiletoDelete] == 1){ // This is a go back to root. [/]
      Serial.println(F("Can't Delete [/]")); 
    }
    else if (TypeIndex[FiletoDelete] == 2){ // This is the previous directory [..]
      Serial.println(F("Can't Delete [..]"));   
    }

    else if (TypeIndex[FiletoDelete] == 3){ // This is a directory
      String OldCurrentDirectory = CurrentDirectory;
      if (CurrentDirectory == "/"){
        CurrentDirectory = CurrentDirectory + FileNametoDelete.substring(1, FileNametoDelete.length()-1);
      }
      else{
        CurrentDirectory = CurrentDirectory + "/" + FileNametoDelete.substring(1, FileNametoDelete.length()-1);
      }
      sd.chdir(CurrentDirectory.c_str(),true);
      Serial.print(F("Deleting Dir: "));   
      Serial.println(CurrentDirectory);
      sd.vwd()->rmRfStar();
      CurrentDirectory = "/";       // Need to go back to root and then back to the dir we were in ???
      sd.chdir(CurrentDirectory.c_str(),true);
      CurrentDirectory = OldCurrentDirectory;
      CurrentFile = 1;
      sd.chdir(CurrentDirectory.c_str(),true);
      SDCardGetDir(0);
      SetCurrentFile();
    }
    else if ((TypeIndex[FiletoDelete] == 10) || (TypeIndex[FiletoDelete] == 11) || (TypeIndex[FiletoDelete] == 12)
      || (TypeIndex[FiletoDelete] == 22) || (TypeIndex[FiletoDelete] == 23) || (TypeIndex[FiletoDelete] == 24)){   // This is a DSK, DDP, ROM, DS2, DS3 or DS4 file
      Serial.print(F("Deleting File: "));   
      Serial.println(FileNametoDelete);
      boolean dirtest = sd.remove(FileNametoDelete.c_str());
      Serial.print(F("Refreshing Directory: "));
      Serial.println(CurrentDirectory);
      sd.chdir(CurrentDirectory.c_str(),true);
      for (int d = 4; d <=7;d++){                                 // Check all of the drives to see if this file was mounted
        unsigned int CurrentlyMountedCheck;
        EEPROM.get( ((d * 400) + 2), CurrentlyMountedCheck);
        if (IndextoDelete ==  CurrentlyMountedCheck){
          StatusSetup(0x43,d); // Set the Status to "no disk"
          EEPROM.write((d * 400), 0); // Write the File Number
          EEPROM.write((d * 400) + 2, 0); // Write the File Number Index
          EEPROM.write((d * 400) + 4, 0); // Write the File Type = 0
          EepromStringWrite((d * 400) + 5, "");
          EepromStringWrite((d * 400) + 105, "");
        }
      }
      SDCardGetDir(0);
      SetCurrentFile();
    }
  }
  else {
    Serial.println(F("Delete Command Error"));
  }
  BlockBuffer[devicenumber-4][0] = 0xB0 + devicenumber; // Set up the header
  BlockBuffer[devicenumber-4][1] = 0x04;   // Tells Adam it's 1024 (0x400) bytes long
  BlockBuffer[devicenumber-4][2] = 0x00;
  for (int i = 3; i <= 1026 ; i++){      // Fill the rest of the buffer with 0x00
    BlockBuffer[devicenumber-4][i] = 0x00;
  }
  BlockBufferChecksum(devicenumber);     // Calculate the checksum before sending
}
void  NewImage(byte devicenumber){             // Command 0xF7
  // data1 =  not used, data2 = not used, data3 = not used
  // Block Buffer Byte 3  = File type
  // Block Buffer Byte 4-5 = File Size in Blocks ( Max 0x3FFF )
  // Block Buffer Byte 6-105 = Filename, doesn't include extension
  interrupts();                          // Enable Interrupts
  byte data1 = (long)WantedBlock >> 16;
  byte data2 = (long)WantedBlock >> 8;
  byte data3  = (long)WantedBlock;
  Serial.println(F("Adam Command: Create Image"));
  unsigned int Blocks = word(BlockBuffer[devicenumber-4][5],BlockBuffer[devicenumber-4][4]);
  Serial.print(F("Number of Blocks: "));
  Serial.println(Blocks);
  unsigned long Filesize = Blocks * 1024.0;
  Serial.print(F("File Size: "));
  Serial.println(Filesize);
  char IncomingFileName[100];
  int FileLength = 0;
  for(int i =6; i<=105; i++){
    if (BlockBuffer[devicenumber-4][i] != 0){
      IncomingFileName[i-6] = BlockBuffer[devicenumber-4][i];
    }
    else{
      FileLength = i-6;
      IncomingFileName[i-6] = 0x00;
      i = 105;
    }
  }
  // See if there are spaces at the end
  for (int i = FileLength-1; i > 0; i--){
    if (IncomingFileName[i] == 0x20){
      IncomingFileName[i] = 0x00;
      FileLength = i+1;
    }
    else{
      FileLength = i+1;
      i = 0;
    }
  }
  if (BlockBuffer[devicenumber-4][3] == 3){    // Create a Dir   
    Serial.println(F("Create a Directory"));
    Serial.print(F("Directory Name: "));
    Serial.println(IncomingFileName);
    if(sd.mkdir(IncomingFileName)){
      StatusSetup(0x40, devicenumber);
    }
    else{
      StatusSetup(0x41, devicenumber);
    }
  }
  else if (BlockBuffer[devicenumber-4][3] == 10){    // Create a Disk  
    Serial.println(F("Create Disk Image"));
    IncomingFileName[FileLength] = '.';
    IncomingFileName[FileLength+1] = 'd';
    IncomingFileName[FileLength+2] = 's';
    IncomingFileName[FileLength+3] = 'k';
    IncomingFileName[FileLength+4] = 0x00;
    Serial.print(F("File Name: "));
    Serial.println(IncomingFileName);
    for (int i = 0; i<=1023;i++){
      BlockBuffer[devicenumber-4][i] = 0xE5;    // Need to re-purpose the blockbuffer.
    }
    if(file.open(IncomingFileName, O_CREAT | O_EXCL | O_WRITE)){
      Serial.print(0);
      Serial.print(F(": "));
      for(unsigned long i = 1; i <= Blocks; i++){
        Serial.print(F("."));
        if ( i % 40 == 0){
          Serial.println();
          Serial.print(i);
          Serial.print(F(": "));
          Serial.flush();
        }
        file.write(BlockBuffer[devicenumber-4], 1024);
      }
      file.close();
      Serial.println();
      StatusSetup(0x40, devicenumber);
    }
    else{
      StatusSetup(0x41, devicenumber);
      Serial.println(F("Error creating file"));
    }
  }
  else if (BlockBuffer[devicenumber-4][3] == 11){    // Create a DDP 
    Serial.println(F("Create DDP Image"));
    IncomingFileName[FileLength] = '.';
    IncomingFileName[FileLength+1] = 'd';
    IncomingFileName[FileLength+2] = 'd';
    IncomingFileName[FileLength+3] = 'p';
    IncomingFileName[FileLength+4] = 0x00;
    Serial.print(F("File Name: "));
    Serial.println(IncomingFileName);
    for (int i = 0; i<=1023;i++){
      BlockBuffer[devicenumber-4][i] = 0xE5;    // Need to re-purpose the blockbuffer. 
    }
    if(file.open(IncomingFileName, O_CREAT | O_EXCL | O_WRITE)){
      for(int i =1; i<=Blocks; i++){
        file.write(BlockBuffer[devicenumber-4], 1024);
      }
      file.close();
      StatusSetup(0x40, devicenumber);
    }
    else{
      StatusSetup(0x41, devicenumber);
      Serial.println(F("Error creating file"));
    }
  }
  else{
    Serial.print(F("Invalid File Type: "));
    Serial.println(BlockBuffer[devicenumber-4][3]);
  }
  sd.chdir(CurrentDirectory.c_str(),true);
  SDCardGetDir(0);
  SetCurrentFile();
}
void  Rename(byte devicenumber){               // Command 0xF8
  // data1 = Not Used, data2 = Not Used , data3 = Not Used
  // Block Buffer Byte 3 = File Type - Requested file type change
  // Block Buffer Byte 4-5 = File Number
  // Block Buffer Byte 6-105 = New Filename, if it has an extension, it will be replaced
  interrupts();                          // Enable Interrupts
  byte data1 = (long)WantedBlock >> 16;
  byte data2 = (long)WantedBlock >> 8;
  byte data3  = (long)WantedBlock;
  Serial.println(F("Adam Command: Rename Image"));
  char IncomingFileName[100];
  unsigned int FiletoRename = word(BlockBuffer[devicenumber-4][5],BlockBuffer[devicenumber-4][4]);
  String OldFileName = GetFileName(FiletoRename,99);
  int FileLength = 0;
  unsigned int IndextoRename = FilesIndex[FiletoRename];
  char ExtTest[5];
  char ExttoClean[5];
  byte filetypechange = BlockBuffer[devicenumber-4][3];
  if (filetypechange == 0x00){                // Check if the file type requested is 0, if so use the existing file type.
    filetypechange = TypeIndex[FiletoRename];
  }
  // Get the incoming file name and the length
  for(int i =6; i<=105; i++){
    if (BlockBuffer[devicenumber-4][i] != 0){
      IncomingFileName[i-6] = BlockBuffer[devicenumber-4][i];
    }
    else{
      FileLength = i-6;
      IncomingFileName[i-6] = 0x00;
      i = 106;
    }
  }
  if ((TypeIndex[FiletoRename] == 10) || (TypeIndex[FiletoRename] == 11) || (TypeIndex[FiletoRename] == 12)
      || (TypeIndex[FiletoRename] == 22) || (TypeIndex[FiletoRename] == 23) || (TypeIndex[FiletoRename] == 24)){   // This is a DSK, DDP, ROM, DS2, DS3 or DS4 file
    // Check what file type it is and assign the extension
    if (filetypechange == 10){
      strcpy(ExttoClean, ".dsk");
    }
    else if (filetypechange == 11){
      strcpy(ExttoClean, ".ddp");
    }
    else if (filetypechange == 12){
      strcpy(ExttoClean, ".rom");
    }
    else if (filetypechange == 22){
      strcpy(ExttoClean, ".ds2");
    }
    else if (filetypechange == 23){
      strcpy(ExttoClean, ".ds3");
    }
    else if (filetypechange == 24){
      strcpy(ExttoClean, ".ds4");
    }
    else {
      strcpy(ExttoClean, ".dsk");
    }
    // See if there are spaces at the end
    for (int i = FileLength-1; i > 0; i--){
      if (IncomingFileName[i] == 0x20){
        IncomingFileName[i] = 0x00;
        FileLength = i+1;
      }
      else{
        FileLength = i+1;
        i = 0;
      }
    }
    // Check if the last 4 char are the extension
    ExtTest[0] = IncomingFileName[FileLength-4];
    ExtTest[1] = IncomingFileName[FileLength-3];
    ExtTest[2] = IncomingFileName[FileLength-2];
    ExtTest[3] = IncomingFileName[FileLength-1];
    ExtTest[4] = IncomingFileName[FileLength];
    strupr(ExtTest);
    strupr(ExttoClean);
    // If so, remove them
    if ((strcmp(ExtTest, ".DSK") == 0) || (strcmp(ExtTest, ".DS2") == 0) || (strcmp(ExtTest, ".DS3") == 0) || 
        (strcmp(ExtTest, ".DS4") == 0) || (strcmp(ExtTest, ".DDP") == 0)|| (strcmp(ExtTest, ".ROM") == 0)){
      IncomingFileName[FileLength-4] = 0x00;
      FileLength = FileLength - 4;
    }
    // See if there are spaces before the end of the file name
    for (int i = FileLength-1; i > 0; i--){         // See if there are spaces....
      if (IncomingFileName[i] == 0x20){
        IncomingFileName[i] = 0x00;
      }
      else{
        i = 0;
      }
    }
    // Add the file extension to the file name
    strlwr(ExttoClean);
    strcat(IncomingFileName, ExttoClean);
    Serial.print(F("Old File Number: "));
    Serial.println(FiletoRename);
    Serial.print(F("Old File Name: "));
    Serial.println(OldFileName);
    Serial.print(F("New File Name: "));
    Serial.println(IncomingFileName);
    if(file.open(sd.vwd(),IndextoRename, O_READ | O_WRITE)){
      if (file.rename(IncomingFileName)){
        StatusSetup(0x40, devicenumber);
      }
      else{
        StatusSetup(0x41, devicenumber);
        Serial.println(F("Error renaming file"));
      }
    }
    else{
      StatusSetup(0x41, devicenumber);
      Serial.println(F("Error opening file for rename"));
    }
    file.close();
  }
  else if (TypeIndex[FiletoRename] == 3){ // This is a directory
    OldFileName.remove(OldFileName.length()-1,1);
    OldFileName.remove(0,1);
    // See if there are spaces at the end
    for (int i = FileLength-1; i > 0; i--){
      if (IncomingFileName[i] == 0x20){
        IncomingFileName[i] = 0x00;
        FileLength = i+1;
      }
      else{
        FileLength = i+1;
        i = 0;
      }
    }
    Serial.print(F("Old Dir Number: "));
    Serial.println(FiletoRename);
    Serial.print(F("Old Dir Name: "));
    Serial.println(OldFileName);
    Serial.print(F("New Dir Name: "));
    Serial.println(IncomingFileName);
    if(sd.rename(OldFileName.c_str(), IncomingFileName)) {
      StatusSetup(0x40, devicenumber);
    }
    else{
      StatusSetup(0x41, devicenumber);
      Serial.println(F("Error renaming dir"));
    }
  }
  sd.chdir(CurrentDirectory.c_str(),true);
  SDCardGetDir(0);
  SetCurrentFile();
  for (int d = 4; d <=7;d++){                                 // Check all of the drives to see if this file was mounted
    unsigned int CurrentlyMountedCheck;
    EEPROM.get( ((d * 400) + 2), CurrentlyMountedCheck);
    if (IndextoRename ==  CurrentlyMountedCheck){
      EEPROM.put((d * 400), FiletoRename);                    // Write the File Number
      EEPROM.put((d * 400) + 2, FilesIndex[FiletoRename]);    // Write the File Number Index
      EEPROM.put((d * 400) + 4, TypeIndex[FiletoRename]);     // Write the File Type
      EepromStringWrite((d * 400) + 5, IncomingFileName);
      EepromStringWrite((d * 400) + 105, CurrentDirectory);
    }
  }
}
void  CutCopyPaste(byte devicenumber){         // Command 0xF9
  // data1 = 1 = Cut, 2 = Copy, 3 = Paste, 4 = Clear Data,  data2 = File Number (High) , data3 = File Number (Low)
  // This is will store the cut or copy information in the EEPROM. And paste to the current directory.
  byte data1 = (long)WantedBlock >> 16;
  byte data2 = (long)WantedBlock >> 8;
  byte data3  = (long)WantedBlock;
  String CurrentFilename = "";
  unsigned int FiletoCopy = word(data2,data3);
  unsigned int IndextoCopy = FilesIndex[FiletoCopy];
  interrupts();                          // Enable Interrupts
  if ((data1 == 1) || (data1 == 2)){       // This is a CUT or COPY
    if (data1 == 1 ){
      Serial.print(F("Cut File: "));
    }
    else{
      Serial.print(F("Copy File: "));
    }
    if (CurrentDirectory == "/"){
     Serial.print(CurrentDirectory);
    }
    else{
     Serial.print(CurrentDirectory + "/");
    }
    CurrentFilename = GetFileName(FiletoCopy,99);
    Serial.println(CurrentFilename);
    EEPROM.put(100, data1);                         // Write the flag to indicate if this is a cut or copy
    EEPROM.put(101, FiletoCopy);                    // Write the File Number
    EEPROM.put(103, IndextoCopy);                   // Write the File Number Index
    EepromStringWrite(105, CurrentFilename);       // Write the file name
    EepromStringWrite(205, CurrentDirectory);       // Write the directory
  }
  else if (data1 == 3){                 // This is a Paste
    SdFile  DistinationFile;
    String SourceFileName = EepromStringRead(105);
    String SourceDir = EepromStringRead(205);
    byte cutorcopy = EEPROM.read(100);
    String SourceFullPath;
    String DesinationFileName;
    byte errorflag = 0;
    if (cutorcopy == 1 ){
      Serial.println(F("Paste Command - Move"));
    }
    else{
      Serial.println(F("Paste Command - Copy"));
    }
    if (SourceDir == "/"){
      SourceFullPath = SourceDir + SourceFileName;
    }
    else{
     SourceFullPath = SourceDir + "/" + SourceFileName;
    }
    DesinationFileName = SourceFileName;
    for(int i = 1; i <=255; i++){                     // See if the file we are trying to create exists.......
      if(sd.exists(DesinationFileName.c_str())){
        Serial.println(F("File Exists - Renaming"));
        DesinationFileName = i + SourceFileName;
      }
      else{
        i = 255;
      }
    }
    Serial.print("Source File:");
    Serial.println(SourceFullPath);
    Serial.print("Destination File:");
    Serial.println(DesinationFileName);
    if(DistinationFile.open(DesinationFileName.c_str(), O_CREAT | O_EXCL | O_WRITE)){    // Open the "to" file for writing
      if(file.open(SourceFullPath.c_str(), O_READ)){    // Open the "from" file for reading
        size_t data; 
        while(file.available()){
          data = file.read(BlockBuffer[devicenumber-4], 1024);
          DistinationFile.write(BlockBuffer[devicenumber-4], 1024);
        }
      }
      else {
        Serial.println(F("Error Opening Source File!"));
        errorflag = 1;
      }
    }
    else {
      Serial.println(F("Error Creating Destination File!"));
      errorflag = 1;
    }
    file.close();
    DistinationFile.close();
    if (cutorcopy == 1){
// This is a cut and the source file needs to be deleted
      word SourceIndex = EEPROM.read(103);
      Serial.print(F("Deleting File: "));   
      Serial.println(SourceFullPath);
      boolean removetest = sd.remove(SourceFullPath.c_str());
      for (int d = 4; d <=7;d++){                                 // Check all of the drives to see if this file was mounted
        unsigned int CurrentlyMountedCheck;
        EEPROM.get( ((d * 400) + 2), CurrentlyMountedCheck);
        if (SourceIndex ==  CurrentlyMountedCheck){
          StatusSetup(0x43,d); // Set the Status to "no disk"
          EEPROM.write((d * 400), 0); // Write the File Number
          EEPROM.write((d * 400) + 2, 0); // Write the File Number Index
          EEPROM.write((d * 400) + 4, 0); // Write the File Type = 0
          EepromStringWrite((d * 400) + 5, "");
          EepromStringWrite((d * 400) + 105, "");
        }
      }
      Serial.println(F("Clearing Cut/Copy Filename"));
      EEPROM.put(100, 0);                         // Write the flag to indicate if this is a cut or copy
      EEPROM.put(101, 0);                    // Write the File Number
      EEPROM.put(103, 0);                   // Write the File Number Index
      EepromStringWrite(105, "");
    }
    if (errorflag == 0){
      StatusSetup(0x40, devicenumber);
    }
    else{
      StatusSetup(0x41, devicenumber);
    }
    sd.chdir(CurrentDirectory.c_str(),true);
    SDCardGetDir(0);
    SetCurrentFile(); 
  }
  else if (data1 == 4){                 // This is a clear
    Serial.print(F("Clearing Cut/Copy Filename"));
    EEPROM.put(100, 0);                    // Write the flag to indicate if this is a cut or copy
    EEPROM.put(101, 0);                    // Write the File Number
    EEPROM.put(103, 0);                   // Write the File Number Index
    EepromStringWrite(105, "");
  }
  BlockBuffer[devicenumber-4][0] = 0xB0 + devicenumber; // Set up the header
  BlockBuffer[devicenumber-4][1] = 0x04;   // Tells Adam it's 1024 (0x400) bytes long
  BlockBuffer[devicenumber-4][2] = 0x00;
  for (int i = 3; i <= 1026 ; i++){       // Fill the rest of the buffer with 0x00
    BlockBuffer[devicenumber-4][i] = 0x00;
  }
  BlockBufferChecksum(devicenumber);     // Calculate the checksum before sending
}
void  Format(byte devicenumber){               // Command 0xFA
  // data1 = not used, must be 0, data2 = 0 on first read, 0xFA on second, data3 = not used, must be 0
  // This format command uses 2 reads in order to initiate the format.
  // First a read to 0xFA000000 and then a read to 0xFA00FA00 to confirm the format.
  // They need to be in order with no block reads between them.
  byte data1 = (long)WantedBlock >> 16;
  byte data2 = (long)WantedBlock >> 8;
  byte data3  = (long)WantedBlock;
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
