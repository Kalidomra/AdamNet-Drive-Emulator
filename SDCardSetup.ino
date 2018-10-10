void SDCardSetup(){                                                // Setup the SD Card
  pinMode(SS, OUTPUT);                     // This makes sure that the default hardware "Slave Select" pin is set to output, even if we use a different pin for chipSelect
                                           // Initialize at the highest speed supported by the board that is
                                           // not over 50 MHz. Try a lower speed if SPI errors occur.
  if (!sd.begin(chipSelect, SD_SCK_MHZ(50))) {
    sd.initErrorHalt();
  }
  // List files in root directory.
  if (!dirFile.open("/", O_READ)) {
    sd.errorHalt("SD Card not present");
  }
  while (numberoffiles < maxfiles && file.openNext(&dirFile, O_READ)) {
    if (!file.isSubDir() && !file.isHidden()) {// Skip directories and hidden files.
      char incomingfilename[255];
      file.getName(incomingfilename,255);
      String testfilename = String(incomingfilename);
      testfilename.trim();
      byte length = testfilename.length();
      String testextension = testfilename.substring(length-4,length);
      testfilename = testfilename.substring(0,length-4);
      testextension.toLowerCase();
      if (testextension == ".dsk"){
        numberoffiles++;
        sdfiles[numberoffiles] = testfilename.substring(0,namelength);
        filesindex[numberoffiles] = file.dirIndex();
        Serial.print(numberoffiles);
        Serial.print(" (");
        Serial.print(filesindex[numberoffiles]);
        Serial.print(")");
        Serial.print(": ");
        Serial.print(sdfiles[numberoffiles]);
        Serial.println();
      }
    }
    file.close();
    }
}