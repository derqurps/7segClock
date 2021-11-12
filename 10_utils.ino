void checkForRestart() {
  if (restartNow) {
    
    NVIC_SystemReset();  // Reset the microcontroller
  }
}
