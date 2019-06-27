/**********************************************************************************************************************************************************************************************************************************
  This routine will first of all wake up the stepper which has been put to 'Sleep' earlier.   Then we will run back the stepper to the end-stop.   All inputs from Screen 2 will be reset to zero ready for fresh inputs.
  If the previous inputs declared the 'IN' 'OUT' or Run to be 'set' this is also set back to false. Finally, the stepper is put back to 'Sleep' to conserve power and heat.
**********************************************************************************************************************************************************************************************************************************/
void runReset() {
  digitalWrite(enablePin, LOW);                                                                             // Wake stepper
  tft.fillRoundRect(168, 218, 144, 44, 4, CYAN);
  tft.setCursor(175, 230);
  tft.print("RUNNING REV");
  while (digitalRead(endStopPin) == 1) {                                                                    // check if limit switch is activated
    RunBack(10);
  }
  tft.fillRoundRect(168, 218, 144, 44, 4, CYAN);
  tft.setCursor(175, 230);
  tft.print("RUNNING FWD");
  for (r = 0; r < z; r++) {                                                                                 // Get well clear of the microswitch (equivalent to 600 FULL steps!)
    RunForward(10);
  }
  r = 0;
  in = 0;
  out = 0;
  stepCount = 0;
  stepsForward = 0;
  stepsBack = 0;
  shutters = 0;
  inSet = false;
  outSet = false;
  runSet = false;
  distance = 0;
  previousDirection = 1;
  tft.fillRoundRect(130, 52, 90, 30, 4, CYAN);                                                              // Stepcount
  tft.fillRoundRect(130, 92, 90, 30, 4, CYAN);                                                              // Shutters
  tft.fillRoundRect(380, 52, 90, 30, 4, CYAN);                                                              // IN
  tft.fillRoundRect(380, 92, 90, 30, 4, CYAN);                                                              // OUT
  tft.fillRoundRect(380, 172, 90, 30, 4, CYAN);                                                             // Distance
  tft.fillRoundRect(168, 218, 144, 44, 4, CYAN);                                                            // Messagebox
  tft.setCursor(210, 230);
  tft.print("HOME");
  digitalWrite(enablePin, HIGH);                                                                            // Put Easydriver back to sleep
}
