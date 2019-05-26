void SDCardSetup(){                                                // Setup the SD Card
  pinMode(SS, OUTPUT);                     // This makes sure that the default hardware "Slave Select" pin is set to output, even if a different pin is used for ChipSelect
  NumberofFiles =0;
  if (!sd.begin(ChipSelect, SD_SCK_MHZ(50))) {// Initialize at the highest speed supported by the board that is not over 50 MHz. Try a lower speed if SPI errors occur.
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(F("   No SD Card"));
    sd.initErrorHalt();
  }
  while (NumberofFiles < MaxFiles && file.openNext(sd.vwd(), O_READ)) {
    if (!file.isSubDir() && !file.isHidden()) {// Skip directories and hidden files.
      char incomingfilename[255];
      file.getName(incomingfilename,255);
      String testfilename = String(incomingfilename);
      testfilename.trim();
      testfilename.toLowerCase();
      BootDisk.toLowerCase();
      byte length = testfilename.length();
      String testextension = testfilename.substring(length-4,length);
      //testfilename = testfilename.substring(0,length-4);   // Remove the extension
      testextension.toLowerCase();
      if ((testextension == ".dsk") || (testextension == ".ddp")){
        NumberofFiles++;
        FilesIndex[NumberofFiles] = file.dirIndex();
        if ((testfilename == BootDisk) && (BootDisk != "")){
          BootDiskIndex = NumberofFiles;
        }
        if (testextension == ".dsk"){
          typeindex[NumberofFiles] = 0;
          Serial.print(F("DSK:"));
        }
        if (testextension == ".ddp"){
          typeindex[NumberofFiles] = 1;
          Serial.print(F("DDP:"));
        }
        Serial.print(NumberofFiles);
        Serial.print(F(":"));
        Serial.print(FilesIndex[NumberofFiles]);
        Serial.print(F(": "));
        Serial.print(testfilename.substring(0,NameLength));
        Serial.println();
      }
    }
  file.close();
  }
}
