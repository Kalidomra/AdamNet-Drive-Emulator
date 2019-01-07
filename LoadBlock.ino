int LoadBlock(unsigned long blocknumber, byte devicenumber){                           // Load block from SD card to buffer array
  digitalWrite(statusled,HIGH);             // Turn on the status LED
  byte currentbyte = 0x00;
  byte checksum = 0x00;
  int remainder = blocknumber % 4;
  switch(devicenumber){
    case 4:
      file.open(&dirFile,filesindex[MountedFile4], O_READ);
      Serial.print("D1: Reading: ");
      Serial.println(blocknumber);
      break;
    case 5:
      file.open(&dirFile,filesindex[MountedFile5], O_READ);
      Serial.print("D2: Reading: ");
      Serial.println(blocknumber);
      break;    
    case 6:
      file.open(&dirFile,filesindex[MountedFile6], O_READ);
      Serial.print("D3: Reading: ");
      Serial.println(blocknumber);
      break;    
    case 7:
      file.open(&dirFile,filesindex[MountedFile7], O_READ);
      Serial.print("D4: Reading: ");
      Serial.println(blocknumber);
      break;    
  }
  blockdata[0] = 0xB0 + devicenumber;
  blockdata[1] = 0x04;
  blockdata[2] = 0x00;
  unsigned long blocklocation = blocknumber*0x00000400;
  file.seekSet(blocklocation);
  for (int i=3; i<=514;i++){
    currentbyte = file.read();
    checksum ^= currentbyte;
    blockdata[i] = currentbyte;
  }
  if (remainder==0||remainder==1){
    file.seekSet((blocknumber*0x400)+(interleave * 0x200));
    for (int i=515; i<=1026;i++){
      currentbyte = file.read();
      checksum ^= currentbyte;
      blockdata[i] = currentbyte;
    }
  }
  else{
    file.seekSet((blocknumber*0x400)-(0x1000-(interleave * 0x200)));
    for (int i=515; i<=1026;i++){
      currentbyte = file.read();
      checksum ^= currentbyte;
      blockdata[i] = currentbyte;
    }
  }
  blockdata[1027] = checksum;
  file.close();
  digitalWrite(statusled,LOW);             // Turn off the status LED  
  return 1;
}
