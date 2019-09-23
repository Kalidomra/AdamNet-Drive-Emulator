void LCDRefresh(){                                                 // Refresh the LCD Display
  String temptext = "";
  byte LCDLength;
  if (LCDScrollOn && LCDTopDelay == 0){
    LCDScroll();                           // Scroll the bottom line of the LCD
  }
  if (LCDTopDelay == 1){
    if ((BootDiskMounted == 1) && (DeviceDisplayed == 4)){
      temptext = "D1:" + BootDisk;
    }
    else{
      sd.chdir(CurrentDirectory.c_str(),true);
      temptext = EepromStringRead((DeviceDisplayed * 400) + 5);
      temptext = String("D") + (DeviceDisplayed - 3) + String(":") + temptext;
    }
    lcd.setCursor(0,0);
    LCDLength = temptext.length();
    if (LCDLength > 16){
      LCDLength = 16;
    }
    for (int i = 0; i < LCDLength; i++) {
      lcd.print(temptext.substring(i,i+1));
    }
    for (int i = LCDLength; i <= 15; i++) {
      lcd.print(" ");
    }
    LCDTopDelay = 0;
  }
  else if(LCDTopDelay > 0){
    LCDTopDelay--;
  }
  if (LCDBottomDelay == 1){
    sd.chdir(CurrentDirectory.c_str(),true);
    LCDBottomText  = GetFileName(CurrentFile,LCDNameLength);
    lcd.setCursor(0,1);
    LCDLength = LCDBottomText.length();
    if (LCDLength > 16){
      LCDLength = 16;
    }
    for (int i = 0; i < LCDLength; i++) {
      lcd.print(LCDBottomText.substring(i,i+1));
    }
    for (int i = LCDLength; i <= 16; i++) {
      lcd.print(" ");
    }
    CurrentLCDDelay = LCDScrollDelayStart;
    LCDScrollLocation = 0;
    LastScrollLCD = millis();
    LCDScrollOn = true;
    LCDBottomDelay = 0;
  }
  else if(LCDBottomDelay > 1){
    LCDBottomDelay--;
  }
}
