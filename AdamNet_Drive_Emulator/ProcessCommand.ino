void ProcessCommand(byte commandin, byte devicenumber){                               // Process Commands
  byte incoming = 0x00;
  byte ack = 0x90 + devicenumber;
  byte transfercommand[8];
  //Serial.print("Command: "); 
  //Serial.println(commandin,HEX);
  if ((commandin >> 4) == 0x01){           // ==============  0x01 - Status Command  ====================================================
    SendArray(status[devicenumber-4],5);                  // Send the status
    incoming = ReceiveByte();              // Receive ACK from Adam
  }
  else if ((commandin >> 4) == 0x0D){      // ==============  0x0D - Ready Command (Are we Ready for a Transfer?)  ======================
    SendByte(ack);                         // Send ACK
    ReceiveArray(transfercommand,8);       // Get the Data transfer command
    // 0 - Transfer Command
    // 1 - High Byte - Length of this data block
    // 2 - Low Byte - Length of this data block
    // 3 - Low Byte - Low Word - Wanted Block
    // 4 - High Byte - Low Word - Wanted Block
    // 5 - Low Byte - High Word - Wanted Block
    // 6 - High Byte - High Word - Wanted Block
    // 7 - Byte for second device
    // 8 - Checksum
    if ((transfercommand[0] >> 4) == 0x06){// ==============  0x06 - Send Command (This tells us the wanted block)  =====================
      SendByte(ack);                       // Tell Adam we received the wanted block
      byte ReadorWrite = ReceiveByte();    // Receive the next byte. This determines if it is a read or a write operation. (0x04 is Read, 0x0D is Write)
      WantedBlock =((long)transfercommand[6] << 24) + ((long)transfercommand[5] << 16) + ((long)transfercommand[4] << 8)  + transfercommand[3];
      if (WantedBlock <= 0x003FFFFF || WantedBlock >= 0xF1000000){
        if ((ReadorWrite >> 4) == 0x04){   // ==============  0x04 - Read Command  ======================================================
          if (WantedBlock != loadedblock[devicenumber-4]){ // If the wanted block is not in the buffer array, we need to load it
            if (WantedBlock <= 0x003FFFFF){
              LoadBlock(WantedBlock, devicenumber);// Load the block from the SD card.
              _delay_us(IODelay);
            }
            else if (WantedBlock >= 0xF1000000){
              SDCommand(transfercommand, devicenumber);// Process the block as an SD Card Command
            }
            loadedblock[devicenumber-4] = WantedBlock;     // Now the block in the array is the wanted block
          }  
          else {                           // We have the block in the array
            SendByte(ack);                 // Tell Adam we have the block and are ready
            incoming = ReceiveByte();      // See what Adam says.....
            if ((incoming >> 4) == 0x03){  // ==============  0x03 - Clear to Send Command  =============================================
              SendArray(blockdata[devicenumber-4],1027);  // Send the whole buffer array. 
              incoming = ReceiveByte();    // See the response. Not sure why. I guess we could add a check and resend. But the loop should take care of that.         
              StatusSetup(0x40,devicenumber);// Set the status to all good
              if (WantedBlock >= 0xF1000000){
                loadedblock[devicenumber-4] = 0xFFFFFFFF;
              }
            }
          }
        }    
        else if ((ReadorWrite >>4) == 0x0D) {// ==============  0x0D - Ready Command (Are we ready for the write?) ========================
          SendByte(ack);                   // Tell Adam we are ready to receive the block write
          ReceiveArray(blockdata[devicenumber-4],1027);   // Receive the block with header and checksum
          byte currentbyte = 0x00;         // Set up for checksum calcs
          byte checksum = 0x00;            // Set up for checksum calcs
          for (int j=3; j<=1026; j++) {    // Calculate the checksum.
            currentbyte = blockdata[devicenumber-4][j];
            checksum ^= currentbyte;
          }
          if (blockdata[devicenumber-4][1027] == checksum){// Check to make sure the received checksum matches the calculated checksum
            if ((WantedBlock == 0x0000FACE) && EnableFACE){      // Received the strange format command
                FACECommand(devicenumber);
            }
            else if (WantedBlock <= 0x003FFFFF){
              SaveBlock(WantedBlock, devicenumber);// Save this block to the SD card
              _delay_us(IODelay);
            }
            else if (WantedBlock >= 0xF1000000){
              SDCommand(transfercommand, devicenumber);  // Process the block as an SD Card Command
            }
            StatusSetup(0x40, devicenumber);               // Set the status to all good
            SendByte(ack);                 // Tell Adam we received a valid block to write
          }
          else {
            Serial.print("D");
            Serial.print(devicenumber-3);
            Serial.print(": Error: Invalid Checksum, Block:");
            Serial.println(WantedBlock);
            StatusSetup(0x41, devicenumber);// Set the status to Bad Block
          }
          loadedblock[devicenumber-4] = 0xFFFFFFFF;                // After a write we need to re-read from the sd card the block if asked.
        }
      }
      else {
      Serial.print("D");
      Serial.print(devicenumber-3);
      Serial.print(": Error: Requested block out of range: ");
      Serial.println(WantedBlock);
      StatusSetup(0x42, devicenumber);              // Set status to no block
      }
    }
    else{
      Serial.print("D");
      Serial.print(devicenumber-3);
      Serial.print(": Error: Expected 0x06 Command, received this: ");// ==============    Unexpected Command  ==============
      Serial.println(transfercommand[0], HEX);
    }
  }
  else {                                   // ==============    Unknown Command  ==============
    Serial.print("D");
    Serial.print(devicenumber-3);
    Serial.print(": Error: Unknown Command: ");
    Serial.println(commandin, HEX);
  }
}
