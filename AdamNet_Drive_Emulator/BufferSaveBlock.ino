void BufferSaveBlock(){                                            // Save Buffer to Device
  byte currentbyte = 0x00;                 // Set up for checksum calcs
  byte checksum = 0x00;                    // Set up for checksum calcs
  for (int j=3; j<=1026; j++) {            // Calculate the checksum.
    currentbyte = BlockBuffer[WantedDevice-4][j];
    checksum ^= currentbyte;
  }
  if (BlockBuffer[WantedDevice-4][1027] == checksum){ // Check to make sure the received checksum matches the calculated checksum
    if ((WantedBlock == 0x0000FACE) && EnableFACE){ // Received the strange format command
      FACECommand(WantedDevice);
    }
    else if (WantedBlock <= 0x003FFFFF){   // ============== Normal SD Card Block Save ==============
      SDSaveBlock(WantedBlock, WantedDevice);// Save this block to the SD card
      _delay_us(IODelay);
    }
    else if ((WantedBlock >= 0xF1000000)   
            && (WantedBlock < 0xFFFFFFFF)){// ============== SD Command Block Save  =================
      SDCommand(WantedDevice);             // Process the block as an SD Card Command
    }
    else {
      Serial.print(F("D"));
      Serial.print(WantedDevice-3);
      Serial.print(F(": Error: Requested block out of range: "));
      Serial.println(WantedBlock);
      StatusSetup(0x42, WantedDevice);     // Set Status to no block
      LoadedBlock[WantedDevice-4] = 0xFFFFFFFF;
    }
  }
  else {                                   // ============== Block is Out of Range ==================
    Serial.print(F("D"));
    Serial.print(WantedDevice-3);
    Serial.print(F(": Error: Invalid Checksum, Block:"));
    Serial.println(WantedBlock);
    StatusSetup(0x41, WantedDevice);       // Set the Status to Bad Block
    byte NacktoAdam[1];
    NacktoAdam[0] = 0xC0 + WantedDevice;         
    AdamNetSend(NacktoAdam,1);

  }
  LoadedBlock[WantedDevice-4] = 0xFFFFFFFF; // After a write need to re-read from the sd card the block if asked.
  if (SDCommandFAConfirm > 0){               // Decrease the confirmation byte for the SD command FA.
    SDCommandFAConfirm--;
  }
   if (SDCommandF6Confirm > 0){               // Decrease the confirmation byte for the SD command F6.
    SDCommandF6Confirm--;
  }
}
