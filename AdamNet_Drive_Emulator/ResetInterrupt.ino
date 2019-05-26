void ResetInterrupt(){                                             // Reset from the Adam  
  TimetoByte = millis();                   // Set the time that the reset came in
  ResetFlag = 1;                           // Set the reset flag
  EIFR = bit (INTF1);                      // Clear flag for any interrupts on INT1 that were triggered while we were in the ISR
}
