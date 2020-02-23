byte ReadButtons(){                                                // Read the Buttons from the ADE
  // 0 = No Key pressed                
  // 1 = Right - Short Press          91 = Right - Long Press
  // 2 = Up
  // 3 = Down
  // 4 = Left - Short Press           94 = Left - Long Press
  // 5 = Select - Short Press         95 = Select - Long Press
  // 6 = Swap Button - Only Digital
  byte DebounceDelay = 50;
  int AnalogButtonState  = 1000;
  byte KeyOutput = 0;
  unsigned long KeyTimer = 0;
  unsigned long LongKeyAnalog = LongKey/20.0; // Analog read is 20 times slower than digital
  unsigned long ButtonDelay = millis() - LastButtonTime;
  byte RightButtonState = digitalRead(RightButtonPin);
  byte UpButtonState = digitalRead(UpButtonPin);
  byte DownButtonState = digitalRead(DownButtonPin);
  byte LeftButtonState = digitalRead(LeftButtonPin);
  byte SelectButtonState = digitalRead(SelectButtonPin);
  byte SwapButtonState = digitalRead(SwapButtonPin);
  if (EnableAnalogButtons){
    AnalogButtonState = analogRead(0);          // Read Analog pin 0 (Takes 110 us)
    if(AnalogButtonState < AnalogTriggerRight){
      delay(DebounceDelay);
      AnalogButtonState = analogRead(0);
      if ((AnalogButtonState < AnalogTriggerRight) && (RepeatKeyFlag == 0)){
        RepeatKeyFlag = 1;
        while ((analogRead(0) < AnalogTriggerRight) && (KeyTimer < LongKeyAnalog)){     // Wait until the button is released or the timer expires
          KeyTimer++;
        }
        if (KeyTimer < LongKeyAnalog){                                                 // Less than the LongKeyAnalog delay = short button press
          KeyOutput = 1;
        }
        else {
          KeyOutput = 91;                                                        // Longer than the LongKeyAnalog delay  = long button press
        }
      }
    }
    else if(AnalogButtonState < AnalogTriggerUp){
      delay(DebounceDelay);
      AnalogButtonState = analogRead(0);
      if (AnalogButtonState < AnalogTriggerUp){
        if(ButtonDelay >= RepeatKeyDelay){
          KeyOutput = 2;
          LastButtonTime = millis();             // Reset the button press timer
        }
      }
    }
    else if(AnalogButtonState < AnalogTriggerDown){
      delay(DebounceDelay);
      AnalogButtonState = analogRead(0);
      if (AnalogButtonState < AnalogTriggerDown){
        if(ButtonDelay >= RepeatKeyDelay){
          KeyOutput = 3;
          LastButtonTime = millis();             // Reset the button press timer
        }
      }
    }
    else if(AnalogButtonState < AnalogTriggerLeft){
      delay(DebounceDelay);
      AnalogButtonState = analogRead(0);
      if ((AnalogButtonState < AnalogTriggerLeft) && (RepeatKeyFlag == 0)){
        RepeatKeyFlag = 1;
        while ((analogRead(0) < AnalogTriggerLeft) && (KeyTimer < LongKeyAnalog)){     // Wait until the button is released or the timer expires
          KeyTimer++;
        }
        if (KeyTimer < LongKeyAnalog){                                                 // Less than the LongKeyAnalog delay = short button press
          KeyOutput = 4;
        }
        else {
          KeyOutput = 94;                                                        // Longer than the LongKeyAnalog delay  = long button press
        }
      }
    }
    else if(AnalogButtonState < AnalogTriggerSelect){
      delay(DebounceDelay);
      AnalogButtonState = analogRead(0);
      if ((AnalogButtonState < AnalogTriggerSelect) && (RepeatKeyFlag == 0)){
        RepeatKeyFlag = 1;
        while ((analogRead(0) < AnalogTriggerSelect) && (KeyTimer < LongKeyAnalog)){     // Wait until the button is released or the timer expires
          KeyTimer++;
        }
        if (KeyTimer < LongKeyAnalog){                                                 // Less than the LongKey delay = short button press
          KeyOutput = 5;
        }
        else {
          KeyOutput = 95;                                                        // Longer than the LongKey delay  = long button press
        }
      }
    }
  }
  if (RightButtonState == LOW){                                    // Right button press
    delay(DebounceDelay);                                                      // Wait the debouncing delay
    if ((digitalRead(RightButtonPin) == LOW) && (RepeatKeyFlag == 0)){         // If it is still low then this is a good button press
      RepeatKeyFlag = 1;
      while((digitalRead(RightButtonPin) == LOW) && (KeyTimer < LongKey)){     // Wait until the button is released or the timer expires
        KeyTimer++;
      }
      if (KeyTimer < LongKey){                                                 // Less than the LongKey delay = short button press
        KeyOutput = 1;
      }
      else {
        KeyOutput = 91;                                                        // Longer than the LongKey delay  = long button press
      }
    }
  }
  else if (UpButtonState == LOW){                                  // Up button press
    delay(DebounceDelay);
    if (digitalRead(UpButtonPin) == LOW){
      if(ButtonDelay >= RepeatKeyDelay){
        KeyOutput = 2;
        LastButtonTime = millis();             // Reset the button press timer
      }
    }
  }
  else if (DownButtonState == LOW){                                // Down button press
    delay(DebounceDelay);
    if (digitalRead(DownButtonPin) == LOW){
      if(ButtonDelay >= RepeatKeyDelay){
        KeyOutput = 3;
        LastButtonTime = millis();             // Reset the button press timer
      }
    }
  }
  else if (LeftButtonState == LOW){                                // Left button press
    delay(DebounceDelay);
    if ((digitalRead(LeftButtonPin) == LOW) && (RepeatKeyFlag == 0)){
      RepeatKeyFlag = 1;
      while((digitalRead(LeftButtonPin) == LOW) && (KeyTimer < LongKey)){     // Wait until the button is released or the timer expires
        KeyTimer++;
      }
      if (KeyTimer < LongKey){                                                 // Less than the LongKey delay = short button press
        KeyOutput = 4;
      }
      else {
        KeyOutput = 94;                                                        // Longer than the LongKey delay  = long button press
      }
    }
  }
  else if (SelectButtonState == LOW){                              // Select button press
    delay(DebounceDelay);
    if ((digitalRead(SelectButtonPin) == LOW) && (RepeatKeyFlag == 0)){
      RepeatKeyFlag = 1;
      while((digitalRead(SelectButtonPin) == LOW) && (KeyTimer < LongKey)){     // Wait until the button is released or the timer expires
        KeyTimer++;
      }
      if (KeyTimer < LongKey){                                                 // Less than the LongKey delay = short button press
        KeyOutput = 5;
      }
      else {
        KeyOutput = 95;                                                        // Longer than the LongKey delay  = long button press
      }
    }
  }
  else if (SwapButtonState == LOW){                               // Swap button press
    delay(DebounceDelay);
    if ((digitalRead(SwapButtonPin) == LOW) && (RepeatKeyFlag == 0)){
      RepeatKeyFlag = 1;
      KeyOutput = 6;
    }
  }
  if ((RightButtonState == HIGH) && (UpButtonState == HIGH) && (DownButtonState == HIGH) &&
      (LeftButtonState == HIGH) && (SelectButtonState == HIGH) && (SwapButtonState == HIGH) &&
      (AnalogButtonState > AnalogTriggerSelect)) {                 // No buttons pressed reset the Repeat Key Flag
    RepeatKeyFlag = 0;
  }
  return KeyOutput;
}
