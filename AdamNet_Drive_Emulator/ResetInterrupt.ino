void ResetInterrupt(){                                             // Reset from the Adam
  if (!DisableNextReset){
    if (bootdiskindex != 0){               // Do we have a boot disk to mount?
      MountedFile[0] = bootdiskindex;
      loadedblock[0] = 0xFFFFFFFF;         // We don't have a block loaded for the new file
      StatusSetup(0x40,4);                 // Set the status to "disk in"
      Serial.print(F("D1: <boot>: "));
      Serial.println(BootDisk);
      refreshscreen = 1;
    }
    Serial.println(F("Reset from AdamNet"));
  }
  else{
    Serial.println(F("Skipping this reset"));
    DisableNextReset=false;
  }
  Serial.println(F("Compatible Block Mode Enabled"));
  CompMode = true;                         // Turn on Compatible Block Mode
  EIFR = bit (INTF1);                      // Clear flag for any interrupts on INT1 that were triggered while we were in the ISR
}