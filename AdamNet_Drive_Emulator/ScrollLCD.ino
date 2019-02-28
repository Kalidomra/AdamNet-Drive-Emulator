void ScrollLCD(){                                                  // Scroll the bottom LCD Line
  unsigned long CurrentScrollDelay = millis() - LastScrollLCD;
  if (CurrentScrollDelay >= CurrentLCDDelay){
    if((LCDScrollLocation +16) >= LCDCurrentText.length()){
      LCDScrollLocation = 0;
      lcd.setCursor(0,1);
      lcd.print(LCDCurrentText.substring(0,40));
      CurrentLCDDelay = LCDScrollDelayStart;
    }
    else{
      LCDScrollLocation++;
      lcd.setCursor(0,1);
      lcd.print(LCDCurrentText.substring(LCDScrollLocation, LCDScrollLocation+16));
      CurrentLCDDelay = LCDScrollDelay;
    }
    LastScrollLCD = millis();
  }
}