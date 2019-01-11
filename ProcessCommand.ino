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
    // 3 - Low Byte - Wanted Block
    // 4 - High Byte - Wanted Block
    // 5-7 ???
    // 8 - Checksum
    if ((transfercommand[0] >> 4) == 0x06){// ==============  0x06 - Send Command (This tells us the wanted block)  =====================
      SendByte(ack);                       // Tell Adam we received the wanted block and other mysterous bytes
      byte ReadorWrite = ReceiveByte();    // Receive the next byte. This determines if it is a read or a write operation. (0x04 is Read, 0x0D is Write)
      WantedBlock = (transfercommand[4]) * 256 + transfercommand[3];
      _delay_us(50);                       // Not sure why, but it won't work without this delay
      if (WantedBlock == 0xFACE){          // Received the strange format command
        Serial.print("Received Format Command for Drive D");
        Serial.println(devicenumber-3);
        file.open(sd.vwd(),filesindex[MountedFile[devicenumber-4]], O_READ);
        unsigned long sizeoffile = file.fileSize();
        file.close();
        unsigned int numberofblocks = sizeoffile/0x400;
        loadedblock[devicenumber-4] = -1;
        Serial.print("Size of file: ");
        Serial.println(sizeoffile);
        Serial.print("Number of Blocks: ");
        Serial.println(numberofblocks);
        for (int i = 0; i<= numberofblocks-1;i++){
          for (int j = 3; j<=1026; j++){
            blockdata[devicenumber-4][j] = 0xe5;
          }
          SaveBlock(i,devicenumber);
        }
        Serial.println("Format complete");
      }
      else{
        if ((ReadorWrite >> 4) == 0x04){   // ==============  0x04 - Read Command  ======================================================
          if (WantedBlock != loadedblock[devicenumber-4]){ // If the wanted block is not in the buffer array, we need to load it
            int result = LoadBlock(WantedBlock, devicenumber);// Load the block from the SD card.
            loadedblock[devicenumber-4] = WantedBlock;     // Now the block in the array is the wanted block
          }  
          else {                           // We have the block in the array
            SendByte(ack);                 // Tell Adam we have the block and are ready
            incoming = ReceiveByte();      // See what Adam says.....
            if ((incoming >> 4) == 0x03){  // ==============  0x03 - Clear to Send Command  =============================================
              //Serial.print("Sending Block: ");
              //Serial.println(WantedBlock);
              //_delay_us(80);
              SendArray(blockdata[devicenumber-4],1027);  // Send the whole buffer array. 
              incoming = ReceiveByte();    // See the response. Not sure why. I guess we could add a check and resend. But the loop should take care of that.         
              StatusSetup(0x40,devicenumber);// Set the status to all good
            }
          }
        }    
        else if ((ReadorWrite >>4) == 0x0D) {// ==============  0x0D - Ready Command (Are we ready for the write?) ========================
          //_delay_us(16);
          SendByte(ack);                   // Tell Adam we are ready to receive the block write
          ReceiveArray(blockdata[devicenumber-4],1027);   // Receive the block with header and checksum
          byte currentbyte = 0x00;         // Set up for checksum calcs
          byte checksum = 0x00;            // Set up for checksum calcs
          for (int j=3; j<=1026; j++) {    // Calculate the checksum.
            currentbyte = blockdata[devicenumber-4][j];
            checksum ^= currentbyte;
          }
          if (blockdata[devicenumber-4][1027] == checksum){// Check to make sure the received checksum matches the calculated checksum
            int result = SaveBlock(WantedBlock, devicenumber);// Save this block to the SD card
            StatusSetup(0x40, devicenumber);               // Set the status to all good
            SendByte(ack);                 // Tell Adam we received a valid block to write
          }
          else {
            StatusSetup(0x41, devicenumber);// Set the status to Bad Block
            Serial.print("D");
            Serial.print(devicenumber-3);
            Serial.println(": Received Block <--- Invalid Checksum!!!!!!!!");
          }
          loadedblock[devicenumber-4] = -1;                // After a write we need to re-read from the sd card the block if asked.
        }
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
