void LCDScroll(){                                                  // Scroll the bottom LCD Line
  unsigned long CurrentScrollDelay = millis() - LastScrollLCD;
  String LCDDisplay = "";
  String LCDTemp = "";
  if (CurrentScrollDelay >= CurrentLCDDelay){
    if((LCDScrollLocation +16) >= LCDBottomText.length()){
      LCDScrollLocation = 0;
      LCDDisplay = LCDBottomText.substring(0,16); 
      CurrentLCDDelay = LCDScrollDelayStart;
    }
    else{
      LCDScrollLocation++;
      LCDDisplay = LCDBottomText.substring(LCDScrollLocation, LCDScrollLocation+16);
      CurrentLCDDelay = LCDScrollDelay;
    } 
    lcd.setCursor(0,1); 
    byte LCDLength = LCDDisplay.length();
    for (int i = 0; i < LCDLength; i++) {
      LCDTemp = LCDDisplay.substring(i,i+1); 
      lcd.print(LCDTemp);
    }
    for (int i = LCDLength; i <= 16; i++) {
      lcd.print(" ");
    }
    LastScrollLCD = millis();
  }
}
