String EepromStringRead (int startinglocation){                    // Read a string from the eeprom
  String readingstring = "";
  int eeprombuffer=0;
  for(int i=0 ; i < NameLength ; i++){
      eeprombuffer = EEPROM.read( startinglocation + i);
      if (eeprombuffer != 0 ){
        readingstring = readingstring+char(eeprombuffer);
      }
    }
    readingstring.trim();
  return readingstring;
}
void EepromStringWrite (int startinglocation, String incomingstring){ // Write a string to the eeprom
  for(int i=0 ; i < NameLength ; i++){
    EEPROM.write( startinglocation+ i,incomingstring[i] );
  }
}