void FACECommand (byte devicenumber){                              // Process a format command
  Serial.print(F("Received Format Command for Drive D"));
  Serial.println(devicenumber-3);
  file.open(sd.vwd(),filesindex[MountedFile[devicenumber-4]], O_READ | O_WRITE);
  unsigned long sizeoffile = file.fileSize();
  unsigned long numberofblocks = sizeoffile/0x400;
  loadedblock[devicenumber-4] = 0xFFFFFFFF;
  Serial.print(F("Size of file: "));
  Serial.println(sizeoffile);
  Serial.print(F("Number of Blocks: "));
  Serial.println(numberofblocks);
  Serial.flush();
  _delay_us(20);
  if (sizeoffile <= 0xFFFFFC00){           // Less than 4gb. Fat32 max is 2 bytes less than full 4gb.
    for (int i = 1; i<= numberofblocks;i++){
      for (int j = 1; j<=1024; j++){
       file.write(0xe5);
      }
      Serial.print(F("."));
      if ( i % 40 == 0){
        Serial.println();
        Serial.flush();
      }
    }
    Serial.println(F("Format complete"));
  }
  else{
    Serial.print(F("D"));
    Serial.print(devicenumber-3);
    Serial.println(F(": Error: File too large to format"));
  }
  file.close();
}