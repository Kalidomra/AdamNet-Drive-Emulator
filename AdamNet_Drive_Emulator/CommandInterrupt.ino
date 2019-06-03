void CommandInterrupt(){                                           // New byte on the AdamNet
  byte TransferCommand[9];
  byte ReadorWrite[1] = {0x00};            // Is the Incoming command a read or a write?
  byte CleartoSend[1] = {0x00};            // Clear to Send response from Adam
  byte AcktoAdam[1] = {0x00};              // Ack to Adam from Device
  byte AckFromAdam[1]= {0x00};             // Ack from Adam to Device
  byte IncomingCommand = 0x00;             // Incoming command from AdamNet
  if ((LoadBufferArrayFlag == 0) && (SaveBufferArrayFlag == 0) && (ResetFlag == 0)){ // If there are no load, saves or resets to complete
    IncomingCommand = 0x00;                // Zero out the IncomingCommand
    // ===============  Already received the transition to the start bit  ===============
    //_delay_us(1);                        // Wait to get to the center, Now at the center of the start bit
    // ===============  Loop Through and read the bits =============== 
    for (byte i=8; i > 0; --i){
      IncomingCommand >>= 1;               // Shift the bits over to the right. This means the first bit will end up at the far right
      _delay_us(15);                       // Wait one bit width
      __asm__("nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t");
      if ((PIND & _BV(PD2))== 0){          // Read AdamNetRx and see if it is high or low
        IncomingCommand |= 0x80;           // If LOW - bitwise OR with 10000000. Basically put a 1 at the left side.
      }
      else{
        IncomingCommand |= 0x00;           // If High - Do this to keep the cycle count the same for 1's and 0's (I think?)
      }
    }
    // ===============  Wait for the stop bit to finish - try 3/4 of a full bit width  ===============   
    _delay_us(13);   
// ======================== Initial Command Byte Processing - This is time critical procesing============================================
    WantedDevice = IncomingCommand & 0xF;
    if ((WantedDevice == 4 && Device4) || (WantedDevice == 5 && Device5) || (WantedDevice == 6 && Device6) || (WantedDevice == 7 && Device7)){
      AcktoAdam[0] = 0x90 + WantedDevice;
      if ((IncomingCommand >> 4) == 0x0D){ // ===================  0x0D - Ready Command (Ready for a Transfer?)  ========================
        _delay_us(150);                    // Wait before sending the ACK. Otherwise Adam could miss it
        AdamNetSend(AcktoAdam,1);          // Send ACK
        AdamNetReceive(TransferCommand,9); // Get the Data transfer command, wanted block, etc
        if ((TransferCommand[0] >> 4) == 0x06){   
          _delay_us(150);                  // Wait before sending the ACK. Otherwise Adam could miss it.
          AdamNetSend(AcktoAdam,1);        // Tell Adam the wanted block info was received
          AdamNetReceive(ReadorWrite,1);   // Get the next byte, this will determine if this is a read or a write.
          WantedBlock =((long)TransferCommand[6] << 24) + ((long)TransferCommand[5] << 16) + ((long)TransferCommand[4] << 8)  + TransferCommand[3];
          if (WantedBlock == 0xDEADBEEF){  // Check if received the special block number, switch to Enhanced Block mode.
            CompMode = false;              // Turn Compatibility Block Mode off.
          }
          if(CompMode == true){            // In Compatibility Block Mode only look at the first 2 bytes of the block number.
            word temp  = (long)WantedBlock;
            WantedBlock = temp;
          }
          if ((ReadorWrite[0] >> 4) == 0x04){ // ================ This is a Read Command  ====================================================
            if (WantedBlock == LoadedBlock[WantedDevice-4]){  // The wanted block is loaded into the buffer
              _delay_us(150);              // Wait before sending the ACK. Otherwise Adam could miss it.
              AdamNetSend(AcktoAdam,1);    // Tell Adam the wanted block is loaded into the buffer
              AdamNetReceive(CleartoSend,1); // See what Adam says
              if ((CleartoSend[0] >> 4) == 0x03){ // ==============  0x03 - Clear to Send Command  =============================================
                AdamNetSend(BlockBuffer[WantedDevice-4],1028); // Send the whole buffer array. 
                AdamNetReceive(AckFromAdam,1);  // See the response.
                if ((AckFromAdam[0]  >> 4) == 0x02){
                  if (WantedBlock > 0x003FFFFF){
                    LoadedBlock[WantedDevice-4] = 0xFFFFFFFF; //Reset the loaded block for DEADBEEF or SD card reads
                  }
                  StatusSetup(0x40,WantedDevice);// Set the Status to all good
                }
                else{
                  Serial.print(F("Error: Sending Block: ")); // Adam did not send back "Acknowledged"
                  Serial.print(WantedBlock);
                  Serial.print(F("   Expected: "));
                  Serial.print((0x20 + WantedDevice),HEX);
                  Serial.print(F("   Received: "));
                  Serial.println(AckFromAdam[0],HEX);
                  StatusSetup(0x41, WantedDevice);  // Set Status to bad block
                  LoadedBlock[WantedDevice-4] = 0xFFFFFFFF;
                }
              }
              else{
                Serial.print(F("Error: Sending Block: ")); // Adam did not send back "Clear to Send"
                Serial.print(WantedBlock);
                Serial.print(F("   Expected: "));
                Serial.print((0x30 + WantedDevice),HEX);
                Serial.print(F("   Received: "));
                Serial.println(CleartoSend[0],HEX);
                StatusSetup(0x41, WantedDevice); // Set Status to bad block
                LoadedBlock[WantedDevice-4] = 0xFFFFFFFF;
              }
            }
            else {
              LoadBufferArrayFlag = 1;     // The wanted block is not in the buffer. Set the flag to load it.
            }
          }
          else if ((ReadorWrite[0] >> 4) == 0x0D){ // ================ This is a Write Command  ====================================================
            _delay_us(150);                // Wait before sending the ACK. Otherwise Adam could miss it
            AdamNetSend(AcktoAdam,1);      // Tell Adam ready to receive the block write
            AdamNetReceive(BlockBuffer[WantedDevice-4],1028); // Receive the block with header and checksum
            _delay_us(150);                // Wait before sending the ACK. Otherwise Adam could miss it
            AdamNetSend(AcktoAdam,1);      // Tell Adam received a block to write
            SaveBufferArrayFlag = 1;       // Set the flag to save the buffer
          }
        }
        else{
          Serial.print(F("D"));             // Adam should have sent a 0x06 command (Send), something is wrong
          Serial.print(WantedDevice-3);
          Serial.print(F(": Error: Expected: "));
          Serial.print(0x60+WantedDevice);
          Serial.print(F(", Received this: "));
          Serial.println(TransferCommand[0],HEX);
          StatusSetup(0x42, WantedDevice); // Set Status to no block
        }
      }
      else if ((IncomingCommand >> 4) == 0x01){ // ==============  0x01 - Status Command  ===============================================
        _delay_us(150);               // Wait before sending the Status. Otherwise Adam could miss it
        AdamNetSend(Status[WantedDevice-4],6); // Send the Status
        AdamNetReceive(AckFromAdam,1);     // Receive ACK from Adam
        if (MountedFile[WantedDevice-4] == 0){ // After sending the Status, it should be reset. Check if there is a disk mounted.
          StatusSetup(0x43, WantedDevice);
        }
        else{
          StatusSetup(0x40, WantedDevice);
        }
      }
      else {                               // ===================  Unknown Command  =====================================================
        Serial.print(F("D"));
        Serial.print(WantedDevice-3);
        Serial.print(F(": Error: Unknown Command: "));
        Serial.println(IncomingCommand, HEX);
      }
      LastCommandTime = millis();          // Reset the Last Command timer
    }
    else {                                 // The incoming byte is not for any of the drives that are enabled
      AdamNetIdle();                       // Wait for the AdamNet to go Idle
    }
    //if (IncomingCommand != 0x41){Serial.println(IncomingCommand,HEX);}
  }
  //EIFR = bit (INTF1);                      // Clear flag for any interrupts on INT1 (Reset) that were triggered while in the ISR
  EIFR = bit (INTF2);                      // Clear flag for any interrupts on INT2 that were triggered while in the ISR
}
