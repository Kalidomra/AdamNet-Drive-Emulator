void EepromStringWrite (int startinglocation, String incomingstring){
  for(int i=0 ; i < namelength ; i++){
      EEPROM.write( startinglocation+ i,incomingstring[i] );
    }
}