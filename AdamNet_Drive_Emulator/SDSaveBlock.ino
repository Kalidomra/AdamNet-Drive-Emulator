int SDSaveBlock(unsigned long blocknumber, byte devicenumber){     // Save the buffer array to the SD card
  int remainder = blocknumber % 4;
  String filedir;
  unsigned int fileindexnumber;
  byte filetype;
  String DebugText;
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
  sd.chdir(filedir.c_str(),true);
  boolean dirtest = file.open(sd.vwd(),fileindexnumber, O_READ | O_WRITE);
  unsigned long sizeoffile = file.fileSize();
  Serial.print(F("D"));
  Serial.print(devicenumber - 3);
  Serial.print(F(": Writing: "));
  Serial.println(blocknumber);
  unsigned long blocklocation = blocknumber*0x00000400;
  if (blocklocation < sizeoffile){
    if (DebugMode){
      DebugText = "Writing:";
      DebugText = DebugText + blocknumber;
      lcd.setCursor(0,0);
      lcd.print(F("                "));
      lcd.setCursor(0,0);
      lcd.print(DebugText);
      LCDTopDelay = LCDDelay;
    }
    file.seekSet(blocklocation);
    if ((filetype == 10) || (filetype == 22) || (filetype == 23) || (filetype == 24)){ // Is this a DSK, DS2, DS3 or DS4?
      for (int i=3; i<=514;i++){
          file.write(BlockBuffer[devicenumber-4][i]);
      }
      if (remainder==0||remainder==1){
        file.seekSet((blocknumber*0x400)+(Interleave * 0x200));
        for (int i=515; i<=1026;i++){
          file.write(BlockBuffer[devicenumber-4][i]);
        }
      }
      else{
        file.seekSet((blocknumber*0x400)-(0x1000-(Interleave * 0x200)));
        for (int i=515; i<=1026;i++){
          file.write(BlockBuffer[devicenumber-4][i]);
        }
      }
    }
    else if (filetype == 11){              // Is this a DDP?
      for (int i=3; i<=1026;i++){
        file.write(BlockBuffer[devicenumber-4][i]);
      }
    }
    else if (filetype == 12){             // Is this a ROM?
      Serial.println(F("Error: Writing to a ROM File not supported"));
    }
    file.close();
    return 1;
  }
  else{
    Serial.print(F("D"));
    Serial.print(devicenumber-3);
    Serial.print(F(": Error saving block - Block location: "));
    Serial.print(blocklocation);
    Serial.print(F(" is larger than file: "));
    Serial.println(sizeoffile);
    file.close();
    StatusSetup(0x42, devicenumber);       // Set Status to no block
    AdamNetIdle();
    return 2;
  }
}
