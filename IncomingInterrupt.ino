void IncomingInterrupt(){                                          // New byte on the AdamNet
  unsigned long elapsedTime =  micros() - LastByteTime;
  if (elapsedTime >= 1000){                // If is has been 1000 microseconds (1 milliseconds) since we received a byte then this should be a command
    IncomingCommand = 0x00;                // Zero out the IncomingCommand
    // ===============  We already received the transition to the start bit  ===============
    //_delay_us(1);                          // Wait to get to the center, We are now at the center of the start bit
    // ===============  Loop Through and read the bits =============== 
    for (byte i=8; i > 0; --i){
      IncomingCommand >>= 1;               // Shift the bits over to the right. This means the first bit we read will end up at the far right
      _delay_us(15);                       // Wait one bit width
      __asm__("nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t");
      if ((PIND & _BV(PD0))== 0){          // Read AdamNetPin and see if it is high or low
        IncomingCommand |= 0x80;           // If LOW - bitwise OR with 10000000. Basically put a 1 at the left side.
      }
      else{
        IncomingCommand |= 0x00;           // If High - Do this to keep the cycle count the same for 1's and 0's (I think?)
      }
    }
    // ===============  Wait for the stop bit to finish - try 3/4 of a full bit width  ===============   
    _delay_us(13);   
    IncomingCommandFlag = 1;               // Set the flag to tell the main loop that we have a command byte
  }
  LastByteTime = micros();                 // If this is not a command byte then we reset the timer
  EIFR = bit (INTF0);                      // Clear flag for any interrupts on INT0 that were triggered while we were in the ISR
}