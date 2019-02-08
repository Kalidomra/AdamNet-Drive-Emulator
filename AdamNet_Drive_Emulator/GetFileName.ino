String GetFileName(unsigned int IncomingFileIndex){                                                // Get the FileName from Index Number
  char incomingfilename[255];
  byte error = 0;
  String testfilename;
  if (IncomingFileIndex == 0){
    return "";
  }
do {
  sd.vwd()->seekSet(IncomingFileIndex);
  byte opentest = file.open(sd.vwd(),IncomingFileIndex, O_READ);
  if (opentest == 0){
    error++;
  }
  opentest = file.getName(incomingfilename,255);
  if (opentest == 0){
    error++;
  }
  testfilename = String(incomingfilename);
  testfilename.trim();
  testfilename = testfilename.substring(0,namelength);  // Trim to the size of the maximum global namelength
  if (error > 10){
    testfilename = " SD Card Error ";
    break;
  }
}while(error != 0);
  file.close();
  return testfilename;
}
