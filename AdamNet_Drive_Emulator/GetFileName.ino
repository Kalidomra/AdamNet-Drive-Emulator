String GetFileName(unsigned int IncomingFileIndex, byte NameLength){ // Get the FileName from Index Number
  char incomingfilename[NameLength+1];
  byte error = 0;
  String testfilename = "";
  if ((TypeIndex[IncomingFileIndex] == 1)){
    return "[/]";
  }
  if ((TypeIndex[IncomingFileIndex] == 2)){
    return "[..]";
  }
  if ((TypeIndex[IncomingFileIndex] == 255)){
    return "";
  }
do {
  //sd.vwd()->seekSet(FilesIndex[IncomingFileIndex]);
  byte opentest = file.open(sd.vwd(),FilesIndex[IncomingFileIndex], O_READ);
  if (opentest == 0){
    error++;
    file.close();
  }
 if  (NameLength < 13){
   opentest = file.getSFN(incomingfilename);
 }
 else{
   opentest = file.getName(incomingfilename,NameLength);
 } 
  if (opentest == 0){
    error++;
    file.close();
  }
  testfilename = String(incomingfilename);
  if (error > 10){
    testfilename = "SD Card Error";
    break;
  }
  file.close();
}while(error != 0);
  file.close();
  if (TypeIndex[IncomingFileIndex] == 3){   // If the filename is a Directory then add []
    testfilename = "[" + testfilename + "]";
  }
  return testfilename;
}
