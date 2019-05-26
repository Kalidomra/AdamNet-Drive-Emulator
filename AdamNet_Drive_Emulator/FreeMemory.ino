int FreeMemory() {                                                 // Free memory available
  extern char *__brkval;
  char top;
  return &top - __brkval;
}