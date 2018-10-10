void StatusSetup(byte statusin){                                   // Setup the Status Array with the Correct Device and CheckSum
  byte currentbyte = 0x00;
  byte checksum = 0x00;
  status[0] = 0x80 + devicenumber;
  status[1] = 0x00;                        // Maximum Msg Length - Low Byte
  status[2] = 0x04;                        // Maximum Msg Length - High Byte
  status[3] = 0x01;                        // Upper = reserved, Lower is transmit code (1 for block mode, 0 for Character mode)
  status[4] = statusin;                    // Status Block( 0x40 = Ok, 0x41 = Bad Block, 0x42 = No Block, 0x43 = No disk, 0x44 = No Drive)
  for (int i=1; i<=4;i++){
    currentbyte = status[i];
    checksum ^= currentbyte;
  }
  status[5] = checksum;
}