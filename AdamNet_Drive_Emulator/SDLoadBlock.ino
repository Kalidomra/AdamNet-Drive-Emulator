int SDLoadBlock(unsigned long blocknumber, byte devicenumber){     // Load block from SD card to buffer array
  digitalWrite(StatusLed[devicenumber-4],HIGH);// Turn on the Status LED
  byte currentbyte = 0x00;
  byte checksum = 0x00;
  int remainder = blocknumber % 4;
  file.open(sd.vwd(),FilesIndex[MountedFile[devicenumber-4]], O_READ);
  unsigned long sizeoffile = file.fileSize();
  Serial.print(F("D"));
  Serial.print(devicenumber - 3);
  Serial.print(F(": Reading: "));
  Serial.println(blocknumber);
  BlockBuffer[devicenumber-4][0] = 0xB0 + devicenumber;
  BlockBuffer[devicenumber-4][1] = 0x04;
  BlockBuffer[devicenumber-4][2] = 0x00;
  unsigned long blocklocation = blocknumber*0x00000400;
  if (blocklocation < sizeoffile){
    file.seekSet(blocklocation);
    if (typeindex[MountedFile[devicenumber-4]] == 0){  // Is this a disk?
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
    }
    else if (typeindex[MountedFile[devicenumber-4]] == 1){  // Is this a DDP?
      for (int i=3; i<=1026;i++){
        currentbyte = file.read();
        checksum ^= currentbyte;
        BlockBuffer[devicenumber-4][i] = currentbyte;
      }
    }
    BlockBuffer[devicenumber-4][1027] = checksum;
    file.close();
    digitalWrite(StatusLed[devicenumber-4],LOW);// Turn off the Status LED
    return 1;
  }
  else{
    Serial.print("D");
    Serial.print(devicenumber-3);
    Serial.print(F(": Error loading block - Block location: "));
    Serial.print(blocklocation);
    Serial.print(F(" is larger than file: "));
    Serial.println(sizeoffile);
    file.close();
    StatusSetup(0x42, devicenumber);       // Set Status to no block
    return 2;
  }
}