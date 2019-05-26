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
    Serial.println("Enhanced Block Mode Enabled");
    BlockBuffer[WantedDevice-4][0] = 0xB0 + WantedDevice;
    BlockBuffer[WantedDevice-4][1] = 0x04;
    BlockBuffer[WantedDevice-4][2] = 0x00;
    for (int r = 3; r<=10;r++){
     BlockBuffer[WantedDevice-4][r] = EEPROM.read(r-3);
    }
    BlockBuffer[WantedDevice-4][11] = Version[0];
    BlockBuffer[WantedDevice-4][12] = Version[1];
    BlockBuffer[WantedDevice-4][13] = Version[2];
    for (int r = 14; r<=1026;r++){
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
}