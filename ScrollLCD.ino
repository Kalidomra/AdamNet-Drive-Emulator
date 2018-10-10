void ScrollLCD(){                                                  // Scroll the bottom LCD Line
  unsigned long CurrentScrollDelay = millis() - LastScrollLCD;
  if (CurrentScrollDelay >= CurrentLCDDelay){
    if((LCDScrollLocation +16) >= sdfiles[currentfile].length()){
      LCDScrollLocation = 0;
      lcd.setCursor(0,1);
      lcd.print(sdfiles[currentfile]);
      CurrentLCDDelay = LCDScrollDelayStart;
    }
    else{
      LCDScrollLocation++;
      lcd.setCursor(0,1);
      lcd.print(sdfiles[currentfile].substring(LCDScrollLocation, LCDScrollLocation+16));
      CurrentLCDDelay = LCDScrollDelay;
    }
    LastScrollLCD = millis();
  }
}