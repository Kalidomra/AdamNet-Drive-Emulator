int AdamNetSend(byte arraytosend[], int lengthofarray){            // Send array out on AdamNet
  for (int i=0; i<lengthofarray ;i++){
// ===============  Send Start bit ===============
    PORTD &= ~_BV(PD0);                       // Set PD0 = HIGH
    _delay_us(15);                           // delay
    __asm__("nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t");
// ===============  Send all of the bits in the Byte ===============
    for (byte mask = 0x01; mask>0; mask <<= 1) {     // iterate through bit mask
      if (arraytosend[i] & mask){          // if bitwise AND resolves to true
        PORTD |= _BV(PD0);                // Set PD0 = LOW
        _delay_us(14);                     // delay
        __asm__("nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t");
      }
      else{                                // if bitwise AND resolves to false
        PORTD &= ~_BV(PD0);                 // Set PD0 = HIGH
        _delay_us(14);                     // delay
        __asm__("nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t");
      }
    }
// ===============  Send Stop Bit ===============  
    PORTD |= _BV(PD0);                    // Set PD0 = LOW
    _delay_us(14);                     // delay
    __asm__("nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t""nop\n\t");

    //_delay_us(15);                         // delay 
    //__asm__("nop\n\t");
  }
}
