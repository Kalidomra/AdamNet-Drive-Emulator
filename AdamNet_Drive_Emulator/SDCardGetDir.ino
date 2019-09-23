void SDCardGetDir(){                                               // Setup the SD Card
  NumberofFiles = 0;                       // Zero out the number of files
  for (int i = 0; i <= MaxFiles; i++){     // Set all of the types and FilesIndex to Max (255 and 65535).
    TypeIndex[i] = 255;
    FilesIndex[i] = 65535;
  }
  String testfilename;
  int DirLevel = GetDirLevel(CurrentDirectory); //The number of dir levels we are in
  if (DirLevel == 1){                      // In the first level of dir. Only add the [..]
    NumberofFiles++;
    TypeIndex[NumberofFiles] = 1;          // TypeIndex 1 is the indication that this is the [..] entry
  }
  else if (DirLevel > 1){                  // This is the second level of directories. Add the [/] and the [..]
    NumberofFiles++;
    TypeIndex[NumberofFiles] = 0;          // TypeIndex 0 is the indication that this is the [/] entry
    NumberofFiles++;
    TypeIndex[NumberofFiles] = 1;          // TypeIndex 1 is the indication that this is the [..] entry
  }
  while (NumberofFiles < MaxFiles && file.openNext(sd.vwd(), O_READ)) {
    if (!file.isHidden()) {                //Skip hidden files.
      char incomingfilename[255];
      file.getName(incomingfilename,255);
      testfilename = String(incomingfilename);
      testfilename.trim();
      testfilename.toLowerCase();
      byte length = testfilename.length();
      String testextension = testfilename.substring(length-4,length);
      //testfilename  = testfilename.substring(0,length-4);   // Remove the extension
      testextension.toLowerCase();
      if (!file.isSubDir()){               // This is not a Dir
        if ((testextension == ".dsk") || (testextension == ".ddp") || (testextension == ".col") || (testextension == ".rom")||(testextension == ".bin")
            || (testextension == ".ds2") || (testextension == ".ds3") || (testextension == ".ds4")){
          NumberofFiles++;
          FilesIndex[NumberofFiles] = file.dirIndex();
          if (testextension == ".dsk"){
            TypeIndex[NumberofFiles] = 10;
          }
          else if (testextension == ".ddp"){
            TypeIndex[NumberofFiles] = 11;
          }
          else if ((testextension == ".col") || (testextension == ".rom")||(testextension == ".bin")){
            TypeIndex[NumberofFiles] = 12;
          }
          else if (testextension == ".ds2"){
            TypeIndex[NumberofFiles] = 22;
          }
          else if (testextension == ".ds3"){
            TypeIndex[NumberofFiles] = 23;
          }
          else if (testextension == ".ds4"){
            TypeIndex[NumberofFiles] = 24;
          }
        }
      }
      else if(file.isSubDir()){            // This is a Dir
        NumberofFiles++;
        FilesIndex[NumberofFiles] = file.dirIndex();
        TypeIndex[NumberofFiles] = 2;
      }
    }
  file.close();
  }
  if (NumberofFiles >= MaxFiles){
    bool lastfile = file.openNext(sd.vwd(), O_READ); // Are there any more files?
    file.close();
    if (lastfile == true){
      Serial.println(F("Warning: Maximum Number of Files Reached"));
      lcd.setCursor(0,1);
      lcd.print(F("                "));
      lcd.setCursor(0,1);
      lcd.print(F("Too Many Files"));
      LCDBottomDelay = LCDDelay;
    }
  } 
// Sort the 2 arrays by the type from low to high. Puts the [..] and [/] first if they exisit for this dir.
  for(int i=1; i < NumberofFiles; i++) {
    for(int o=1; o < NumberofFiles; o++) {
      if (TypeIndex[o] > TypeIndex[o+1]){
        byte temp1 = TypeIndex[o];
        TypeIndex[o] = TypeIndex[o+1];
        TypeIndex[o+1] = temp1;
        unsigned int temp2 = FilesIndex[o];
        FilesIndex[o] = FilesIndex[o+1];
        FilesIndex[o+1] = temp2;
      }
    }
  }
  for (int i = 1; i <= NumberofFiles; i++){
    Serial.print(i);
    Serial.print(F(":"));
    Serial.print(FilesIndex[i]);
    Serial.print(F(":"));
    Serial.print(TypeIndex[i]);
    Serial.print(F(": "));
    Serial.println(GetFileName(i,99));
  }
}
