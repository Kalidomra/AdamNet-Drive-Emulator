void ProcessReset(){                                               // Process a Reset on AdamNet
  unsigned long TimeSinceReset = 0;
  String DebugText;
  while (((PIND & _BV(PD2)) == 1) && (TimeSinceReset <= 175)){ // Wait for PD2 to go LOW
    TimeSinceReset = millis() - TimetoByte; // Calculate the time since the reset
  }
  while (((PIND & _BV(PD2)) == 0) && (TimeSinceReset <= 175)){ // Wait for PD2 to go HIGH, this is the start of the first byte since reset
    TimeSinceReset = millis() - TimetoByte; // Calculate the time since the reset
  }
  if (TimeSinceReset <= 5){
    TimeSinceReset = 160;
  }
  if (DoubleReset == true){
    Serial.println(F("Double Reset Detected - Treating as Hard Reset"));
  }
  if ((((TimeSinceReset <= 175) && (TimeSinceReset >= 153)) || ((TimeSinceReset <= 23) && (TimeSinceReset >= 15)))|| DoubleReset == true){
    if ((BootDiskExists == 1) && (BootDiskEnabled == 1)) {              // If there is a boot disk then mount it.
      LoadedBlock[0] = 0xFFFFFFFF;         // Set he loaded block to unloaded.
      StatusSetup(0x40, 4);                // Set the Status to "disk in"
      BootDiskMounted = 1;
      Serial.println(F("Mounting Boot Disk Image"));
      Serial.print(F("D1: <boot>: "));
      Serial.println(BootDisk);
    }
    DebugText = "Hard Reset: ";
    Serial.print(F("Hard Reset from AdamNet: "));
    Serial.print(TimeSinceReset);
    Serial.println(F(" ms"));
    CompMode = true;
    Serial.println(F("Compatible Block Mode Enabled"));
  }
  else if ((TimeSinceReset < 33) && (TimeSinceReset > 5)){
    // BootDiskMounted = 0;
    DebugText = "Soft Reset: ";
    Serial.print(F("Soft Reset from AdamNet: "));
    Serial.print(TimeSinceReset);
    Serial.println(F(" ms"));
    CompMode = true;
    Serial.println(F("Compatible Block Mode Enabled"));
    DoubleReset = true;
  } 
 // else{
 //  DebugText = "Adam Off: ";
 //  Serial.println(F("Adam Powered Off"));
 // }
  if ((DebugMode) && (DebugText != "")){
    DebugText = DebugText + TimeSinceReset;
    lcd.clear();
    lcd.setCursor(0,1);
    lcd.print(DebugText);
    LCDTopDelay = 1;
    LCDBottomDelay = LCDDelay;
  }
  else{
    LCDTopDelay = 1;
    LCDBottomDelay = 1;
  }
  LCDScrollOn = true; 
}
