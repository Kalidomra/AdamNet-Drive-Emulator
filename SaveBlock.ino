int SaveBlock(unsigned long blocknumber, byte devicenumber){                // Save the buffer array to the SD card
  digitalWrite(statusled,HIGH);             // Turn on the status LED
  int remainder = blocknumber % 4;
  switch(devicenumber){
  case 4:
    file.open(&dirFile,filesindex[MountedFile4], O_READ | O_WRITE);
    Serial.print("D1: Writing: ");
    Serial.println(blocknumber);
    break;
  case 5:
    file.open(&dirFile,filesindex[MountedFile5], O_READ | O_WRITE);
    Serial.print("D2: Writing: ");
    Serial.println(blocknumber);
    break;
  case 6:
    file.open(&dirFile,filesindex[MountedFile6], O_READ | O_WRITE);
    Serial.print("D3: Writing: ");
    Serial.println(blocknumber);
    break;
  case 7:
    file.open(&dirFile,filesindex[MountedFile7], O_READ | O_WRITE);
    Serial.print("D4: Writing: ");
    Serial.println(blocknumber);
    break;

  }
  unsigned long blocklocation = blocknumber*0x00000400;
  file.seekSet(blocklocation);
  for (int i=3; i<=514;i++){
      file.write(blockdata[i]);
  }
  if (remainder==0||remainder==1){
    file.seekSet((blocknumber*0x400)+(interleave * 0x200));
    for (int i=515; i<=1026;i++){
      file.write(blockdata[i]);
    }
  }
  else{
    file.seekSet((blocknumber*0x400)-(0x1000-(interleave * 0x200)));
    for (int i=515; i<=1026;i++){
      file.write(blockdata[i]);
    }
  }
  file.close();
  digitalWrite(statusled,LOW);             // Turn off the status LED  
  return 1;
}
