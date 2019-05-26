int SDSaveBlock(unsigned long blocknumber, byte devicenumber){     // Save the buffer array to the SD card
  digitalWrite(StatusLed[devicenumber-4],HIGH);// Turn on the Status LED
  int remainder = blocknumber % 4;
  file.open(sd.vwd(),FilesIndex[MountedFile[devicenumber-4]], O_READ | O_WRITE);
  unsigned long sizeoffile = file.fileSize();
  Serial.print(F("D"));
  Serial.print(devicenumber - 3);
  Serial.print(F(": Writing: "));
  Serial.println(blocknumber);
  unsigned long blocklocation = blocknumber*0x00000400;
  if (blocklocation < sizeoffile){
    file.seekSet(blocklocation);
    if (typeindex[MountedFile[devicenumber-4]] == 0){  // Is this a disk?
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
    if (typeindex[MountedFile[devicenumber-4]] == 1){  // Is this a DDP?
      for (int i=3; i<=1026;i++){
        file.write(BlockBuffer[devicenumber-4][i]);
      }
    }
    file.close();
    digitalWrite(StatusLed[devicenumber-4],LOW);// Turn off the Status LED  
    return 1;
  }
  else{
    Serial.print("D");
    Serial.print(devicenumber-3);
    Serial.print(F(": Error saving block - Block location: "));
    Serial.print(blocklocation);
    Serial.print(F(" is larger than file: "));
    Serial.println(sizeoffile);
    file.close();
    StatusSetup(0x42, devicenumber);       // Set Status to no block
    return 2;
  }
}