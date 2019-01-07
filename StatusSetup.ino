void StatusSetup(byte statusin,byte device){                                   // Setup the Status Array with the Correct Device and CheckSum
  byte currentbyte = 0x00;
  byte checksum = 0x00;
  switch (device){
   case 4:
     status4[0] = 0x80 + 4;
     status4[1] = 0x00;                        // Maximum Msg Length - Low Byte
     status4[2] = 0x04;                        // Maximum Msg Length - High Byte
     status4[3] = 0x01;                        // Upper = reserved, Lower is transmit code (1 for block mode, 0 for Character mode)
     status4[4] = statusin;                    // Status Block( 0x40 = Ok, 0x41 = Bad Block, 0x42 = No Block, 0x43 = No disk, 0x44 = No Drive)
     for (int i=1; i<=4;i++){
       currentbyte = status4[i];
       checksum ^= currentbyte;
      }
     status4[5] = checksum;
     break;
   case 5:
     status5[0] = 0x80 + 5;
     status5[1] = 0x00;                        // Maximum Msg Length - Low Byte
     status5[2] = 0x04;                        // Maximum Msg Length - High Byte
     status5[3] = 0x01;                        // Upper = reserved, Lower is transmit code (1 for block mode, 0 for Character mode)
     status5[4] = statusin;                    // Status Block( 0x40 = Ok, 0x41 = Bad Block, 0x42 = No Block, 0x43 = No disk, 0x44 = No Drive)
     for (int i=1; i<=4;i++){
       currentbyte = status5[i];
       checksum ^= currentbyte;
     }
     status5[5] = checksum;
     break;
   case 6:
     status6[0] = 0x80 + 6;
     status6[1] = 0x00;                        // Maximum Msg Length - Low Byte
     status6[2] = 0x04;                        // Maximum Msg Length - High Byte
     status6[3] = 0x01;                        // Upper = reserved, Lower is transmit code (1 for block mode, 0 for Character mode)
     status6[4] = statusin;                    // Status Block( 0x40 = Ok, 0x41 = Bad Block, 0x42 = No Block, 0x43 = No disk, 0x44 = No Drive)
     for (int i=1; i<=4;i++){
       currentbyte = status6[i];
       checksum ^= currentbyte;
     }
     status6[5] = checksum;
     break;
   case 7:
     status7[0] = 0x80 + 7;
     status7[1] = 0x00;                        // Maximum Msg Length - Low Byte
     status7[2] = 0x04;                        // Maximum Msg Length - High Byte
     status7[3] = 0x01;                        // Upper = reserved, Lower is transmit code (1 for block mode, 0 for Character mode)
     status7[4] = statusin;                    // Status Block( 0x40 = Ok, 0x41 = Bad Block, 0x42 = No Block, 0x43 = No disk, 0x44 = No Drive)
     for (int i=1; i<=4;i++){
       currentbyte = status7[i];
       checksum ^= currentbyte;
     }
     status7[5] = checksum;
     break;
  }
}

  
