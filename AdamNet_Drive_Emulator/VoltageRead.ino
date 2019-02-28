void VoltageRead(){                                                // Read the Voltage and Display
  float ADEVolts;
  //float AdamVolts;
  while(1){
    ADEVolts = readVcc()/1000.0;  
    Serial.print(F("ADE Vcc = "));
    Serial.println(ADEVolts);
    delay(50); 
    lcd.clear();
    /*AdamVolts = analogRead(A15) * (5.0/1023.0);
    Serial.print(F("Adam Vcc = "));
    if(AdamVolts > 3){
      Serial.println(AdamVolts); 
    }
    else{
      Serial.println(F("Low"));
    }
    lcd.setCursor(0,0);  
    lcd.print(F("Adam Vcc = "));
    if(AdamVolts > 3){
      lcd.print(AdamVolts); 
    }
    else{
      lcd.print(F("Low"));
    } */
    lcd.setCursor(0,1);
    lcd.print(F("ADE Vcc = "));
    lcd.print(ADEVolts);    
    delay(2000);
  }
}
long readVcc() {
  ADMUX = _BV(REFS0) | _BV(MUX4) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  ADCSRB = 0;
  delay(2); // Wait for Vref to settle
  ADCSRA |= _BV(ADSC); // Start conversion
  while (bit_is_set(ADCSRA,ADSC)); // measuring
  uint8_t low  = ADCL; // must read ADCL first - it then locks ADCH  
  uint8_t high = ADCH; // unlocks both
  long result = (high<<8) | low;
  result = 1125300L / result; // Calculate Vcc (in mV); 1125300 = 1.1*1023*1000
  return result; // Vcc in millivolts
}