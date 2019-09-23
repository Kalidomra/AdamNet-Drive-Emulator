int SDLoadBlock(unsigned long blocknumber, byte devicenumber){     // Load block from SD card to buffer array
  byte currentbyte = 0x00;
  byte checksum = 0x00;
  int remainder = blocknumber % 4;
  String filedir;
  unsigned int fileindexnumber;
  unsigned long sizeoffile;
  unsigned long blocklocation;
  byte filetype;
  String DebugText;
  bool OutofRange = false;
  if ((BootDiskMounted == 1) && (devicenumber == 4)) {
    filedir = BootDiskDir;
    fileindexnumber = BootDiskIndex;
    filetype = BootDiskType;
  }
  else{
    filedir = EepromStringRead((devicenumber * 400) + 105);
    EEPROM.get((devicenumber * 400) + 2, fileindexnumber);
    filetype = EEPROM[(devicenumber * 400) + 4];
  }
  Serial.print(F("D"));
  Serial.print(devicenumber - 3);
  Serial.print(F(": Reading: "));
  Serial.println(blocknumber);
  if (DebugMode){
    DebugText = "Reading:";
    DebugText = DebugText + blocknumber;
    lcd.setCursor(0,0);
    lcd.print(F("                "));
    lcd.setCursor(0,0);
    lcd.print(DebugText);
    LCDTopDelay = LCDDelay;
  }
  BlockBuffer[devicenumber-4][0] = 0xB0 + devicenumber;
  BlockBuffer[devicenumber-4][1] = 0x04;
  BlockBuffer[devicenumber-4][2] = 0x00;
  if ((filetype == 10) || (filetype == 22) || (filetype == 23) || (filetype == 24)){ // Is this a DSK, DS2, DS3 or DS4?
    sd.chdir(filedir.c_str(),true);
    boolean dirtest =  file.open(sd.vwd(),fileindexnumber, O_READ);
    sizeoffile = file.fileSize();
    blocklocation = blocknumber*0x00000400;
    if (blocklocation < sizeoffile){
      file.seekSet(blocklocation);
      for (int i=3; i<=514;i++){
        currentbyte = file.read();
        checksum ^= currentbyte;
        BlockBuffer[devicenumber-4][i] = currentbyte;
      }
      if (remainder==0||remainder==1){
        file.seekSet((blocknumber*0x400)+(Interleave * 0x200));
        for (int i=515; i<=1026;i++){
          currentbyte = file.read();
          checksum ^= currentbyte;
          BlockBuffer[devicenumber-4][i] = currentbyte;
        }
      }
      else{
        file.seekSet((blocknumber*0x400)-(0x1000-(Interleave * 0x200)));
        for (int i=515; i<=1026;i++){
          currentbyte = file.read();
          checksum ^= currentbyte;
          BlockBuffer[devicenumber-4][i] = currentbyte;
        }
      }
      BlockBuffer[devicenumber-4][1027] = checksum;
      file.close();
      return 1;
    }
    else{
     OutofRange = true;
    }
  }
  else if (filetype == 11){              // Is this a DDP?
    sd.chdir(filedir.c_str(),true);
    boolean dirtest =  file.open(sd.vwd(),fileindexnumber, O_READ);
    sizeoffile = file.fileSize();
    blocklocation = blocknumber*0x00000400;
    if (blocklocation < sizeoffile){
      file.seekSet(blocklocation);
      for (int i=3; i<=1026;i++){
        currentbyte = file.read();
        checksum ^= currentbyte;
        BlockBuffer[devicenumber-4][i] = currentbyte;
      }
      BlockBuffer[devicenumber-4][1027] = checksum;
      file.close();
      return 1;
    }
    else{
     OutofRange = true;
    }
  }
  else if (filetype == 12){              // Is this a ROM?
    if(blocknumber == 0x00){             // This is a generated block 0 for a ROM file
      byte block0code[18] = {0x3A,0x6F,0xFD,0x01,0x00,0x00,0x11,0x01,    // This is the boot block it just loads block 1 into 0x2000 and then jumps to 0x2000
                             0x00,0x21,0x00,0x20,0xCD,0xF3,0xFC,0xC3,
                             0x00,0x20};
      for (int i=3; i<=20;i++){
        currentbyte = block0code[i-3];
        checksum ^= currentbyte;
        BlockBuffer[devicenumber-4][i] = currentbyte;
      }
      for (int i=21; i<=1026;i++){
        currentbyte = 0x00;
        checksum ^= currentbyte;
        BlockBuffer[devicenumber-4][i] = currentbyte;
      }
      BlockBuffer[devicenumber-4][1027] = checksum;
      return 1;
    }
    if(blocknumber == 0x01){             // This is a generated block 1 for a ROM file
      byte block1code[57] = {0x11,0x02,0x00,0x21,0x00,0x40,0x3A,0x6F,  // This code will load the rom file from block 2 and above into 0x4000
                             0xFD,0x01,0x00,0x00,0xCD,0xF3,0xFC,0x3E,  // Then the PCB's are move to 0x2100 and the AdamNet scanned
                             0x21,0xBB,0xCA,0x1D,0x20,0x01,0x00,0x04,  // (Thanks to Milli for this information)
                             0x09,0x13,0xC3,0x06,0x20,0x21,0x00,0x21,  // It will then bank switch in OS7 and move the rom code to 0x8000.
                             0xCD,0x7B,0xFC,0xCD,0x8A,0xFC,0x3E,0x03,  // finally it jumps to 0x0000 to start execution of the CV rom.
                             0xCD,0x14,0xFD,0x21,0xFF,0xBF,0x11,0xFF,
                             0xFF,0x01,0x00,0x80,0xED,0xB8,0xC3,0x00,
                             0x00};                     
      sd.chdir(filedir.c_str(),true);
      boolean dirtest =  file.open(sd.vwd(),fileindexnumber, O_READ);
      byte ROMfilesize = ceil((file.fileSize())/0x400);
      if (ROMfilesize > 32){
        Serial.println(F("Warning: ROM file larger than 32k, loading 32k"));
        ROMfilesize = 32;
      }
      block1code[16] = ROMfilesize + 1;     // Change the number of blocks to read to the actual ROM size + 1.
      Serial.print(F("ROM File Size: "));
      Serial.print(ROMfilesize);
      Serial.println(F("k"));
      for (int i=3; i<=59;i++){
        currentbyte = block1code[i-3];
        checksum ^= currentbyte;
        BlockBuffer[devicenumber-4][i] = currentbyte;
      }
      for (int i=60; i<=1026;i++){
        currentbyte = 0x00;
        checksum ^= currentbyte;
        BlockBuffer[devicenumber-4][i] = currentbyte;
      }
      BlockBuffer[devicenumber-4][1027] = checksum;
      file.close();
      return 1;
    }
    else{                                // Block 2 and above is mapped to the actual ROM file.
      sd.chdir(filedir.c_str(),true);
      boolean dirtest =  file.open(sd.vwd(),fileindexnumber, O_READ);
      unsigned long actualblocknumber = blocknumber - 2;
      sizeoffile = file.fileSize();
      blocklocation = actualblocknumber*0x00000400;
      if (blocklocation < sizeoffile){
        file.seekSet(blocklocation);
        for (int i=3; i<=1026;i++){
          currentbyte = file.read();
          checksum ^= currentbyte;
          BlockBuffer[devicenumber-4][i] = currentbyte;
        }
        BlockBuffer[devicenumber-4][1027] = checksum;
        file.close();
        return 1;
      }
      else if ((blocknumber*0x00000400) < 0x28000){   // This will fill the remainder of the fake disk with 0's
        for (int i=3; i<=1026;i++){
          currentbyte = 0x00;
          checksum ^= currentbyte;
          BlockBuffer[devicenumber-4][i] = currentbyte;
        }
        BlockBuffer[devicenumber-4][1027] = checksum;
        file.close();
        return 1;
      }
      else{
       OutofRange = true;
      }
    }
  }
  if (OutofRange == true){
    Serial.print(F("D"));
    Serial.print(devicenumber-3);
    Serial.print(F(": Error loading block - Block location: "));
    Serial.print(blocklocation);
    Serial.print(F(" is larger than file: "));
    Serial.println(sizeoffile);
    file.close();
    AdamNetIdle();
    StatusSetup(0x42, devicenumber);       // Set Status to no block
    return 2;
  }
}
