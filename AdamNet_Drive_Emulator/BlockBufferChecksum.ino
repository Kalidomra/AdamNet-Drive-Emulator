void BlockBufferChecksum(byte device){                             // Calculate the Checksum on the Block Data
  byte currentbyte = 0x00;
  byte checksum = 0x00;
  for (int i=3; i<=1026;i++){
    currentbyte = BlockBuffer[device-4][i];
    checksum ^= currentbyte;
  }
  BlockBuffer[device-4][1027] = checksum;
}