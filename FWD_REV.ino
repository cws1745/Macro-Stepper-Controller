

/***********************************************************************************************************************************************************************************************************************************
  Essentially the same as the Reverse function. There may be a significant number of Forward steps required therefore a lengthy time pressing the >> button - whereas any use of the << button for over-runs is likely to be limited.
  To make things more efficient we introduce the ability to toggle the Steps button to Distance - this then allows the use of the Fwd/Rev/Jog buttons to increment the distance to be travelled in 1/10th millimetre steps.
  The Distance input will set the OUT point only on the basis that setting the In point will be done manually or as the result of an earlier return to the IN point.   Any backlash as a result of a change in direction will be
  taken up by a run forward for the appropriate and measured number of backlash steps (intBacklash) as detailed in the switch - case in selecting microstep sizes.
 ***********************************************************************************************************************************************************************************************************************************/
void forwardButton() {
  if (distanceControl == true) {
    previousDirection = 2;
    distanceCounterUp();
  }
  if (runSet == false && inSet == false && stepControl == true && previousDirection == 0) {           // If Run already set then stepper should be disabled. Otherwise setting IN
    for (int d = 0; d < intBacklash; d++) {                                                           // Previous direction Reverse so immediate backlash
      RunForward(5);                                                                                  // Take up the backlash based on current microstep value but do not count steps
    }
    RunForward(jogSpeed);                                                                             // Backlash taken up so now we move forward
    stepsForward++;                                                                                   // Increment count
    tft.fillRoundRect(380, 52, 90, 30, 4, WHITE);
    tft.setCursor(400, 60);
    tft.print(stepsForward - stepsBack);                                                              // Display count
  }
  else if (runSet == false && inSet == false && stepControl == true && previousDirection == 1) {      // The previous direction was also Forwards so no backlash problems
    RunForward(jogSpeed);
    stepsForward++;
    tft.fillRoundRect(380, 52, 90, 30, 4, WHITE);
    tft.setCursor(400, 60);
    tft.print(stepsForward - stepsBack);
  }
  else if (inSet == true && outSet == false && stepControl == true && previousDirection == 0) {       // Setting OUT position and previous direction was in Reverse
    for  (int e = 0; e < intBacklash; e++) {                                                          // Previous direction Reverse so immediate backlash
      RunForward(5);                                                                                  // Take up the backlash based on current microstep value but do not count steps
    }
      RunForward(jogSpeed);                                                                           // Backlash taken up so now we move forward
      stepsForward++;                                                                                 // Increment count
      tft.fillRoundRect(380, 92, 90, 30, 4, WHITE);
      tft.setCursor(400, 100);
      tft.print(stepsForward - stepsBack);                                                            // Display count
  }
  else if (inSet == true && outSet == false && stepControl == true && previousDirection == 1) {       // The last run was in the Forward direction also so no problem with backlash
    RunForward(jogSpeed);
    stepsForward++;
    tft.fillRoundRect(380, 92, 90, 30, 4, WHITE);
    tft.setCursor(400, 100);
    tft.print(stepsForward - stepsBack);
  }
}

/***********************************************************************************************************************************************************************************************************************************
   Similar to forward_btn.
 ***********************************************************************************************************************************************************************************************************************************/
void reverseButton() {
  if (distanceControl == true) {
    previousDirection = 2;
    distanceCounterDown();
  }
  if (digitalRead(endStopPin) == 1 && (stepsForward - stepsBack) > 0) {                               // Read HIGH from end-stop to prevent Runback() past 0
    if (runSet == false && inSet == false && stepControl == true && previousDirection == 1) {         // Change of direction. Immediate Backlash problem
      for (int f = 0; f < intBacklash; f++) {                                                         // Previous direction Reverse so immediate backlash
        RunBack(5);                                                                                   // Take up the backlash based on current microstep value but do not count steps
      }
      RunBack(jogSpeed);                                                                              // Now start to count Reverse steps after backlash taken up
      stepsBack++;
      tft.fillRoundRect(380, 52, 90, 30, 4, WHITE);
      tft.setCursor(400, 60);
      tft.print(stepsForward - stepsBack);
    }
    else if (inSet == false && stepControl == true && previousDirection == 0) {                       // Setting IN. Previous direction was Reverse. No backlash problem
      RunBack(jogSpeed);
      stepsBack++;                                                                                    // Count the number of steps in Reverse
      tft.fillRoundRect(380, 52, 90, 30, 4, WHITE);
      tft.setCursor(400, 60);
      tft.print(stepsForward - stepsBack);                                                            // No need to account for backlash
    }
    else if (runSet == false && inSet == true && stepControl == true && previousDirection == 1) {     // Setting OUT. Change of direction
      if ((stepsForward - stepsBack) > in) {
        for (int g = 0; g < intBacklash; g++) {                                                       // Previous direction Forwards so immediate backlash.
          RunBack(5);                                                                                 // Take up the backlash based on current microstep value but do not count steps
        }
        RunBack(jogSpeed);                                                                            // Now start to count Reverse steps after backlash taken up
        stepsBack++;
        tft.fillRoundRect(380, 92, 90, 30, 4, WHITE);
        tft.setCursor(400, 100);
        tft.print(stepsForward - stepsBack);
      }
    }
    else if (runSet == false && inSet == true && stepControl == true && previousDirection == 0) {     // Setting OUT. No change of direction.   No backlash problem
      if ((stepsForward - stepsBack) > in) {
        RunBack(jogSpeed);
        stepsBack++;                                                                                  // Count the number of steps in Reverse
        tft.fillRoundRect(380, 92, 90, 30, 4, WHITE);
        tft.setCursor(400, 100);
        tft.print(stepsForward - stepsBack);                                                          // No need to account for backlash
      }
    }
  }
}
