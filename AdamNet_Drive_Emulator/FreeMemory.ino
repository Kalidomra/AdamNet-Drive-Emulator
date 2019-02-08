int FreeMemory() {
  extern char *__brkval;
  char top;
  return &top - __brkval;
}
