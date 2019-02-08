String EepromStringRead (int startinglocation){
  String readingstring = "";
  int eeprombuffer=0;
  for(int i=0 ; i < namelength ; i++){
      eeprombuffer = EEPROM.read( startinglocation+ i);
      if (eeprombuffer != 0 ){
        readingstring = readingstring+char(eeprombuffer);
      }
    }
    readingstring.trim();
  return readingstring;
}