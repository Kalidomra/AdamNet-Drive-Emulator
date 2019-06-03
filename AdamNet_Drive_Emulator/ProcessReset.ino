void ProcessReset(){                                               // Process a Reset on AdamNet
  unsigned long TimeSinceReset = 0;
  while (((PIND & _BV(PD2)) == 1) && (TimeSinceReset <= 175)){ // Wait for PD2 to go LOW
    TimeSinceReset = millis() - TimetoByte;  // Calculate the time since the reset
  }
  while (((PIND & _BV(PD2)) == 0) && (TimeSinceReset <= 175)){ // Wait for PD2 to go HIGH, this is the start of the first byte since reset
    TimeSinceReset = millis() - TimetoByte;  // Calculate the time since the reset
  }
  //Serial.print("Time: ");
  //Serial.println(TimeSinceReset);
  if (TimeSinceReset < 3){
    TimeSinceReset = 160;
  }
  if ((TimeSinceReset <= 175) && (TimeSinceReset >= 153)){
      Serial.println(F("Hard Reset from AdamNet"));
      if (BootDiskIndex != 0){           // If there is a boot disk, then mount it.
        MountedFile[0] = BootDiskIndex;
        LoadedBlock[0] = 0xFFFFFFFF;     // Reset the loaded block
        StatusSetup(0x40,4);             // Set the Status to "disk in"
        Serial.println(F("Mounting Boot Disk Image"));
        Serial.print(F("D1: <boot>: "));
        Serial.println(BootDisk);
      }
      refreshscreen = 1;  
  }
  else if ((TimeSinceReset < 33) && (TimeSinceReset > 5)){
    Serial.println(F("Soft Reset from AdamNet"));
  }  
  else{
   Serial.println(F("Adam Powered Off")); 
  }
}
