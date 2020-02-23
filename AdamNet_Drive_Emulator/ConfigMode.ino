void ConfigMode(){                                                 // Configuration Mode
  byte LCDRefresh = 1;
  const String RootMenu[6] = {"Exit","D1: ","D2: ","D3: ","D4: ", "Debug Mode: "};
  byte MenuItem = 1;
  Device4 = false;
  Device5 = false;
  Device6 = false;
  Device7 = false;
  DebugMode = false;
  void(* resetFunc) (void) = 0;
  if (EEPROM.read(4) == 1){
    Device4 = true;
  }
  if (EEPROM.read(5) == 1){
    Device5 = true;
  }
  if (EEPROM.read(6) == 1){
    Device6 = true;
  }
  if (EEPROM.read(7) == 1){
    Device7 = true;
  }
  if (EEPROM.read(33) == 1){
    DebugMode = true;
  }
  while(1){
    byte ButtonPressed = ReadButtons();
    if (ButtonPressed == 1){               // Right button pressed
      if (MenuItem == 1){
        if(Device4 == true){
          Device4 = false;
          EEPROM.write(4,0);
        }
        else{
          Device4 = true;
          EEPROM.write(4,1);
        }
      }
      if (MenuItem == 2){
        if(Device5 == true){
          Device5 = false;
          EEPROM.write(5,0);
        }
        else{
          Device5 = true;
          EEPROM.write(5,1);
        }
      }
      if (MenuItem == 3){
        if(Device6 == true){
          Device6 = false;
          EEPROM.write(6,0);
        }
        else{
          Device6 = true;
          EEPROM.write(6,1);
        }
      }
      if (MenuItem == 4){
        if(Device7 == true){
          Device7 = false;
          EEPROM.write(7,0);
        }
        else{
          Device7 = true;
          EEPROM.write(7,1);
        }
      }

      if (MenuItem == 5){
        if(DebugMode == true){
          DebugMode = false;
          EEPROM.write(33,0);
        }
        else{
          DebugMode = true;
          EEPROM.write(33,1);
        }
      }
      if (MenuItem == 0){
        resetFunc();
      }
      LCDRefresh = 1;
    }
    else if (ButtonPressed == 2){          // Up button pressed
      if (MenuItem == 0){
        MenuItem = 5;
      }
      else{
        MenuItem--;
      }
      LCDRefresh = 1;
    }
    else if (ButtonPressed == 3){          // Down button pressed
      if (MenuItem == 5){
        MenuItem = 0;
      }
      else{
        MenuItem++;
      }
      LCDRefresh = 1;
    }
    if (LCDRefresh == 1){
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print(F("Device Config"));
      lcd.setCursor(0,1);
      lcd.print(RootMenu[MenuItem]);
      if (MenuItem == 1){
        if(Device4 == true){
          lcd.print(F("On"));
        }
        else{
          lcd.print(F("Off"));
        }
      }
      else if (MenuItem == 2){
        if(Device5 == true){
          lcd.print(F("On"));
        }
        else{
          lcd.print(F("Off"));
        }
      }
      else if (MenuItem == 3){
        if(Device6 == true){
          lcd.print(F("On"));
        }
        else{
          lcd.print(F("Off"));
        }
      }
      else if (MenuItem == 4){
        if(Device7 == true){
          lcd.print(F("On"));
        }
        else{
          lcd.print(F("Off"));
        }
      }
      else if (MenuItem == 5){
        if(DebugMode == true){
          lcd.print(F("On"));
        }
        else{
          lcd.print(F("Off"));
        }
      }
      LCDRefresh = 0;
      delay(200);
    }
  }
}
