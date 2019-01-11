void SDCardSetup(){                                                // Setup the SD Card
  pinMode(SS, OUTPUT);                     // This makes sure that the default hardware "Slave Select" pin is set to output, even if we use a different pin for chipSelect
  if (!sd.begin(chipSelect, SD_SCK_MHZ(50))) {// Initialize at the highest speed supported by the board that is not over 50 MHz. Try a lower speed if SPI errors occur.
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("   No SD Card");
    sd.initErrorHalt();
  }
  while (numberoffiles < maxfiles && file.openNext(sd.vwd(), O_READ)) {
    if (!file.isSubDir() && !file.isHidden()) {// Skip directories and hidden files.
      char incomingfilename[255];
      file.getName(incomingfilename,255);
      String testfilename = String(incomingfilename);
      testfilename.trim();
      byte length = testfilename.length();
      String testextension = testfilename.substring(length-4,length);
      testfilename = testfilename.substring(0,length-4);
      testextension.toLowerCase();
      if (testextension == ".dsk" || testextension == ".ddp"){
        numberoffiles++;
        filesindex[numberoffiles] = file.dirIndex();
        if (testextension == ".dsk"){
          typeindex[numberoffiles] = 0;
          Serial.print("DSK:");
        }
        if (testextension == ".ddp"){
          typeindex[numberoffiles] = 1;
          Serial.print("DDP:");
        }
        Serial.print(numberoffiles);
        Serial.print(":");
        Serial.print(filesindex[numberoffiles]);
        Serial.print(": ");
        Serial.print(testfilename.substring(0,namelength));
        Serial.println();
      }
    }
  file.close();  
  }
}
