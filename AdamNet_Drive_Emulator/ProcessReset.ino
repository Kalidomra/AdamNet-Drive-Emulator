void ProcessReset(){                                               // Process a Reset on AdamNet
  unsigned long TimeSinceReset = 0;
  while (((PIND & _BV(PD2)) == 0) && (TimeSinceReset <= 170)){ // Wait for PD2 to go HIGH, this is the start of the first byte since reset
    TimeSinceReset = millis() - TimetoByte;  // Calculate the time since the reset
  }
  if ((TimeSinceReset >= 150) || (TimeSinceReset <= 10)){
    if (TimeSinceReset <= 170){
        Serial.println(F("Hard Reset from AdamNet"));
        if (BootDiskIndex != 0){           // If there is a boot disk, then mount it.
          MountedFile[0] = BootDiskIndex;
          LoadedBlock[0] = 0xFFFFFFFF;     // Reset the loaded block
          StatusSetup(0x40,4);             // Set the Status to "disk in"
          Serial.println(F("Mounting Boot Disk Image"));
          Serial.print(F("D1: <boot>: "));
          Serial.println(BootDisk);
        }
        Serial.println(F("Compatible Block Mode Enabled"));
        CompMode = true;                   // Turn on Compatible Block Mode
        refreshscreen = 1;  
    }
    else{
      Serial.println(F("Adam Powered Off")); 
    }
  }
  else {
    Serial.println(F("Soft Reset from AdamNet"));
    Serial.println(F("Compatible Block Mode Enabled"));
    CompMode = true;                       // Turn on Compatible Block Mode
  }
}