void BufferLoadBlock(){                                            // Load data into the Device Buffer
  if (WantedBlock <= 0x003FFFFF){          // ============== Normal SD Card Block Load ==============
    SDLoadBlock(WantedBlock, WantedDevice);// Load the block from the SD card.
    _delay_us(IODelay);
    LoadedBlock[WantedDevice-4] = WantedBlock; // Now the block in the array is the wanted block
  }
  else if ((WantedBlock >= 0xF1000000) 
          && (WantedBlock < 0xFFFFFFFF)){  // ============== SD Command Block Load  =================
    SDCommand(WantedDevice);               // Process the block as an SD Card Command
    LoadedBlock[WantedDevice-4] = WantedBlock; // Now the block in the array is the wanted block
  }
  else if (WantedBlock == 0xDEADBEEF){     // ============== Enhanced Mode Block Load ===============
    Serial.println(F("Enhanced Block Mode Enabled"));
    BlockBuffer[WantedDevice-4][0] = 0xB0 + WantedDevice;
    BlockBuffer[WantedDevice-4][1] = 0x04;
    BlockBuffer[WantedDevice-4][2] = 0x00;
    for (int r = 3; r<=10;r++){
     BlockBuffer[WantedDevice-4][r] = EEPROM.read(r-3);
    }
    BlockBuffer[WantedDevice-4][11] = EEPROM.read(16);  //
    BlockBuffer[WantedDevice-4][12] = EEPROM.read(17);  // Version
    BlockBuffer[WantedDevice-4][13] = EEPROM.read(18);  //
    BlockBuffer[WantedDevice-4][14] = EEPROM.read(32);  // LCD Name Length
    BlockBuffer[WantedDevice-4][15] = highByte(NumberofFiles);
    BlockBuffer[WantedDevice-4][16] = lowByte(NumberofFiles);
    BlockBuffer[WantedDevice-4][17] = 0;
    BlockBuffer[WantedDevice-4][18] = 0;
    BlockBuffer[WantedDevice-4][19] = 0;
    BlockBuffer[WantedDevice-4][20] = 0;
    BlockBuffer[WantedDevice-4][21] = 0;
    BlockBuffer[WantedDevice-4][22] = 0;
   for (int r = 23; r<=(23+CurrentDirectory.length());r++){ // Current Directory
      BlockBuffer[WantedDevice-4][r] = CurrentDirectory.charAt(r-23);
    }
   for (int r = (23 + CurrentDirectory.length()+1); r<=1026;r++){
     BlockBuffer[WantedDevice-4][r] = 0;
   }
    BlockBufferChecksum(WantedDevice);
    LoadedBlock[WantedDevice-4] = WantedBlock; // Now the block in the array is the wanted block
  }
  else {                                   // ============== Block is Out of Range ==================
    Serial.print(F("D"));
    Serial.print(WantedDevice-3);
    Serial.print(F(": Error: Requested block out of range: "));
    Serial.println(WantedBlock);
    StatusSetup(0x42, WantedDevice);       // Set Status to no block
    LoadedBlock[WantedDevice-4] = 0xFFFFFFFF;
  }
  if (SDCommandFAConfirm > 0){               // Decrease the confirmation byte for the SD command FA.
    SDCommandFAConfirm--;
  }
}
