void FACECommand (byte devicenumber){                              // Process a format command
  byte filetype = EEPROM[(devicenumber * 400) + 4];
  Serial.print(F("Received Format Command for Drive D"));
  Serial.println(devicenumber-3);
  if (filetype == 12){
    Serial.println(F("Error: Formatting a ROM File not supported"));
  }
  else{
    String filedir;
    unsigned int fileindexnumber;
    filedir = EepromStringRead((devicenumber * 400) + 105);
    EEPROM.get((devicenumber * 400) + 2, fileindexnumber);
    sd.chdir(filedir.c_str(),true);
    file.open(sd.vwd(),fileindexnumber, O_READ | O_WRITE);
    unsigned long sizeoffile = file.fileSize();
    unsigned long numberofblocks = sizeoffile/0x400;
    LoadedBlock[devicenumber-4] = 0xFFFFFFFF;
    Serial.print(F("Size of file: "));
    Serial.println(sizeoffile);
    Serial.print(F("Number of Blocks: "));
    Serial.println(numberofblocks);
    Serial.flush();
    _delay_us(20);
    if (sizeoffile <= 0xFFFFFC00){           // Less than 4gb. Fat32 max is 2 bytes less than full 4gb.
     byte tempbuff[512];
     for (int i = 0; i<=511;i++){
      tempbuff[i] = 0xe5;
     }
     for (int i = 1; i<= numberofblocks*2;i++){
       file.write(tempbuff, 512);
       file.sync();
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
}
