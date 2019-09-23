void AdamNetIdle(){                                                // Wait for AdamNet to go Idle
  byte IdleDetected = 0;
  int IdleCount = 0;
  int IdleTimeout = 900;                    // 1905 = 2ms, 2380 = 2.5ms, 2870 = 3ms
  do{
    while ((PIND & _BV(PD2)) == 1){        // Wait for AdamNet to go LOW
    }
    IdleCount = 0;
    while (((PIND & _BV(PD2)) == 0)  && (IdleDetected == 0)){  // While AdamNet is Low and count hasn't timed out.
      if (IdleCount > IdleTimeout){
        IdleDetected = 1;
      }
      else{
        IdleCount++;
      }
    }
  }while (IdleDetected == 0);
}
