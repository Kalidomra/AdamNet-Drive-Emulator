unsigned int GetFileNumber(unsigned int IncomingFileNumber,        // Returns the File Number for disk swaps
                           String IncomingDir, String WantedExtension){
  unsigned int FoundIndex;
  String IncomingFileName = GetFileName(IncomingFileNumber,99);    // Get the filename for mounted disk
  IncomingFileName  = IncomingFileName.substring(0,IncomingFileName.length()-4); // Remove the extension
  String WantedFileName = IncomingFileName + WantedExtension;      // Set the wanted filename
  WantedFileName.toLowerCase();                                    // Move the entire name to lowercase
  sd.chdir(IncomingDir.c_str(),true);                              // Change to the directory
  while (file.openNext(sd.vwd(), O_READ)) {                        // Loop through the directory
    if (!file.isSubDir() && !file.isHidden()) {                    // Skip hidden files and directories.
      char incomingfilename[255];
      file.getName(incomingfilename,255);
      String testfilename = String(incomingfilename);
      testfilename.trim();
      testfilename.toLowerCase();
      if (testfilename == WantedFileName){                         // If we found the file then we know the Index Number
        FoundIndex = file.dirIndex();
        break;
      }
    }
    file.close();
  }
  file.close();
  for (unsigned int i = 0; i <= MaxFiles; i++){                    // Loop through the index array to get the found file number
    if (FoundIndex == FilesIndex[i]){
      return i;
    }
  }
  return 0;                                                        // If we didn't find anything return 0
}
