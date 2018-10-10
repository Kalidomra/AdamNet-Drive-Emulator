int SaveBlock(unsigned long blocknumber){                // Save the buffer array to the SD card
  digitalWrite(statusled,HIGH);             // Turn on the status LED
  int remainder = blocknumber % 4;
  file.open(&dirFile,filesindex[MountedFile], O_READ | O_WRITE);
  unsigned long blocklocation = blocknumber*0x00000400;
  Serial.print("Writing: ");
  Serial.println(blocknumber);
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