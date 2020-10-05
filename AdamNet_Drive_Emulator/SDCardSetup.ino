void SDCardSetup(){                                                // Setup the SD Card
  pinMode(SS, OUTPUT);                     // This makes sure that the default hardware "Slave Select" pin is set to output, even if a different pin is used for ChipSelect
  if (!sd.begin(ChipSelect, SD_SCK_MHZ(50))) {// Initialize at the highest speed supported by the board that is not over 50 MHz. Try a lower speed if SPI errors occur.
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(F("   No SD Card"));
    sd.initErrorHalt();
  }         
  Serial.print(F("Changing to the Initial Dir: "));
  Serial.println(CurrentDirectory);
  sd.chdir(CurrentDirectory.c_str(),true);
}
