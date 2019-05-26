int AdamNetReceive(byte arraytoreceive[], int lengthofarray){      // Receive array from AdamNet
  // ==================== Read all of the bytes required into the array =======================
  for (int j=0; j<lengthofarray ;j++){
    byte incomingbyte= 0;
// ===============  Wait for Start bit ===============
    while ((PIND & _BV(PD2)) == 0){        // Wait for PD2 to go HIGH
    }
    _delay_us(8);                          // Wait to get to the center, Now at the center of the start bit
    __asm__("nop\n\t");
// ===============  Loop Through and read the bits =============== 
    for (byte i=8; i > 0; --i){
      incomingbyte >>= 1;                  // Shift the bits over to the right. This means the first bit will end up at the far right
      _delay_us(15);                       // Wait one bit width
      __asm__("nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t");
      if ((PIND & _BV(PD2))== 0){          // If PD2 is LOW then this is a "1" bit
        incomingbyte |= 0x80;              // bitwise OR with 10000000. Basically put a 1 at the left side.
      }
      else{                                // Else this is a "0" bit
        incomingbyte |= 0x00;              // Do this to keep the cycle count the same for 1's and 0's (I think?)
      }
    }
// ===============  Wait for the stopbit to finish - try 3/4 of a full bit width  =============== 	
    _delay_us(13);
// ===============  Set the array byte to the incoming byte ===============
    arraytoreceive[j] = incomingbyte;
  }
}