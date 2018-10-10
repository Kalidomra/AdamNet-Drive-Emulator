int LoadBlock(unsigned long blocknumber){                           // Load block from SD card to buffer array
  digitalWrite(statusled,HIGH);             // Turn on the status LED
  byte currentbyte = 0x00;
  byte checksum = 0x00;
  int remainder = blocknumber % 4;
  file.open(&dirFile,filesindex[MountedFile], O_READ);
  blockdata[0] = 0xB0 + devicenumber;
  blockdata[1] = 0x04;
  blockdata[2] = 0x00;
  unsigned long blocklocation = blocknumber*0x00000400;
  Serial.print("Reading: ");
  Serial.println(blocknumber);
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