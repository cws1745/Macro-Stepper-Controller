/**********************************************************************************************************************************************************************************************************************************
  We are at the 'IN' point as set .  We must first fire the shutter at this point as this is the first point of focus for the image stack, then decrement the number of shutters to be taken.  We will then step through a loop the
  size of which has been calculated by the method  stepsBetweenShutters().   Each iteration of the loop will result in a shutter actuation.   Within this loop we  will run the stepper forwards by the value of stepsPerLoop - a
  value calculated by the same method stepsBetweenShutters().   We will thus ensure that an image is taken at each plane of focus throughout the overall run.  The need to use only integer values for the loop and step
  calculations may have resulted in a difference between the actual steps between the 'IN' and 'OUT' points - this is taken in account at the end of the loops with a final run for this residual number of steps and a final
  shutter actuation.   At the end of the Run the stepper is put back to 'Sleep'.
***********************************************************************************************************************************************************************************************************************************/
void PCRun() {
  tft.fillRoundRect(168, 218, 144, 44, 4, CYAN);
  tft.setCursor(175, 230);
  tft.print("RUNNING FWD");
  tempShutters = shutters;                                                                                // Store shutter value for repeat runs
  tempCount = stepCount;
  stepsTaken = 0;
  ShutterFire();                                                                                          // Shutter must be fired at start of the Run
  shutters--;
  tft.fillRoundRect(130, 92, 90, 30, 4, CYAN);                                                            // Shutters
  tft.setCursor(150, 100);
  tft.print(shutters);
  int loopTemp = loops;
  while (loopTemp > 0) {
    int stepTemp = stepsPerLoop;
    while (stepTemp > 0) {
      RunForward(20);
      stepsTaken++;
      tempCount--;
      tft.fillRoundRect(130, 52, 90, 30, 4, CYAN);                                                        // Steps
      tft.setCursor(150, 60);
      tft.print(tempCount);
      stepTemp--;
    }
    if (fireShutter == 1) {                                                                               // If no Flash then delay needed, otherwise Flash Delay serves to allow vibrations to settle
    delay(stepDelay);                                                                                     // Delay to allow any vibrations to subside when no flash used
    }
    ShutterFire();
    loopTemp--;
    shutters--;
    tft.fillRoundRect(130, 92, 90, 30, 4, CYAN);                                                          // Shutters
    tft.setCursor(150, 100);
    tft.print(shutters);
  }
  totalStepsTaken = (loops * stepsPerLoop);                                                               // There may be residual steps still to take
  if (totalStepsTaken < stepCount) {
    while (totalStepsTaken < stepCount) {                                                                 // There are residual steps
      RunForward(20);
      tempCount--;
      totalStepsTaken++;                                                                                  // At this point totalStepsTaken must equal stepCount and tempCount is nil
    }
    if (fireShutter == 1) {
    delay(stepDelay);                                                                                     // Delay when no flash to allow any vibrations to subside
    }
    ShutterFire();                                                                                        // Now at the end of the Run, last shutter
    shutters--;                                                                                           // Shutter count must be nil
  }
  tft.fillRoundRect(130, 52, 90, 30, 4, CYAN);
  tft.setCursor(150, 60);
  tft.print(stepCount);                                                                                   // Steps if run is to be repeated
  tft.fillRoundRect(130, 92, 90, 30, 4, CYAN);
  tft.setCursor(150, 100);
  tft.print(shutters);                                                                                    // Shutters if run is to be repeated
  tft.fillRoundRect(168, 218, 144, 44, 4, CYAN);
  tft.setCursor(175, 230);
  tft.print("RUNNING REV");
  long totalStepsTakenTemp = totalStepsTaken + intBacklash;                                               // We have to run back allowing for backlash
  while (totalStepsTakenTemp != 0) {                                                                      // Run back to exact point where run started
    RunBack(10);
    totalStepsTakenTemp--;
  }
  for  (int h = 0; h < intBacklash; h++) {                                                                // Previous direction Reverse so immediate backlash
  RunForward(5);                                                                                          // Take up the backlash from the reverse move back to IN
  }
  shutters = tempShutters;                                                                                // Restore the original value for shutters
  tft.fillRoundRect(130, 92, 90, 30, 4, CYAN);                                                            // Shutters
  tft.setCursor(150, 100);
  tft.print(shutters);
  tft.fillRoundRect(168, 218, 144, 44, 4, CYAN);
  tft.setCursor(200, 230);
  tft.print("RUN SET");
  digitalWrite(enablePin, HIGH);                                                                          // Put stepper to Sleep
}
