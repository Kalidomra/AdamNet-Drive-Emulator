void EepromStringWrite(int add,String data){                       // Write a string to the eeprom
  int _size = data.length();
  int i;
  for(i=0;i<_size;i++)
  {
    EEPROM.write(add+i,data[i]);
  }
  EEPROM.write(add+_size,'\0');            //Add termination null character for String Data
}

String EepromStringRead(int add){          // Read a string from the eeprom
  int i;
  char data[100]; //Max 100 Bytes
  int len=0;
  unsigned char k;
  k=EEPROM.read(add);
  while(k != '\0' && len<500)              //Read until null character
  {    
    k=EEPROM.read(add+len);
    data[len]=k;
    len++;
  }
  data[len]='\0';
  return String(data);
}

int GetDirLevel(String s) {
  int count = 0;
  for (int i = 0; i < s.length(); i++){
    if (s[i] == '/') count++;
  }
  if (s.length() == 1){
    count = 0;
  }
  return count;
}

void SetCurrentFile(){
  int DirLevel = GetDirLevel(CurrentDirectory); //The number of dir levels we are in
  if (DirLevel == 1){
    CurrentFile = 2;
  }
  else if (DirLevel > 1){
    if (NumberofFiles > 2){
      CurrentFile = 3;
    }
    else{
      CurrentFile = 2;
    }
  }
  else{
   CurrentFile = 1;
  }
}
int FreeMemory() {                         // Free memory available
  extern char *__brkval;
  char top;
  return &top - __brkval;
}
void BlockBufferChecksum(byte device){     // Calculate the Checksum on the Block Data
  byte currentbyte = 0x00;
  byte checksum = 0x00;
  for (int i=3; i<=1026;i++){
    currentbyte = BlockBuffer[device-4][i];
    checksum ^= currentbyte;
  }
  BlockBuffer[device-4][1027] = checksum;
}