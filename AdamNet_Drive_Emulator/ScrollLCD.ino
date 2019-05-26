void ScrollLCD(){                                                  // Scroll the bottom LCD Line
  unsigned long CurrentScrollDelay = millis() - LastScrollLCD;
  String LCDDisplay = "";
  String LCDTemp = "";
  if (CurrentScrollDelay >= CurrentLCDDelay){
    if((LCDScrollLocation +16) >= LCDCurrentText.length()){
      LCDScrollLocation = 0;
      LCDDisplay = LCDCurrentText.substring(0,16); 
      CurrentLCDDelay = LCDScrollDelayStart;
    }
    else{
      LCDScrollLocation++;
      LCDDisplay = LCDCurrentText.substring(LCDScrollLocation, LCDScrollLocation+16);
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