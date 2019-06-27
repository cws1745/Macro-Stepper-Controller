/**********************************************************************************************************************************************************************************************************************************
  This routine will first of all wake up the stepper.   Then we will run back the stepper to the end-stop, the stepper is the run forwards to clear the microswitch and finally put back to 'Sleep' to conserve power and heat.
  Thr run forwards will be for the equivalent of 600 full steps i.e. 3mm.   This is achieved by the variable 'z' which defaults to 600 but is changed by the microstep button selection.
**********************************************************************************************************************************************************************************************************************************/
void startUp() {
  digitalWrite(enablePin, LOW);                                                                             // Wake stepper
  while (digitalRead(endStopPin) == 1) {                                                                    // check if limit switch is activated
    RunBack(10);
  }
  z = 600;
  for (r = 0; r < z; r++) {                                                                                 // Get well clear of the microswitch (600 FULL steps!)
    RunForward(10);
  }
  r = 0;                                                                                                    // Reset r to zero to allow later runReset()
  digitalWrite(enablePin, HIGH);                                                                            // Put Easydriver back to sleep
}

/**********************************************************************************************************************************************************************************************************************************
  This routine is used by the Run control buttons e.g. Forward, Reverse and Jog buttons as well as the PCRun function. It checks we are clear of the end-stop and runs the stepper back as called. Parameter sets delay i.e. speed.
***********************************************************************************************************************************************************************************************************************************/
void RunBack(int b) {
  digitalWrite(enablePin, LOW);                                                                             // Wake stepper
  if (digitalRead(endStopPin) == 1) {                                                                       // check if limit switch is activated
    digitalWrite(dirPin, HIGH);                                                                             // (HIGH = anti-clockwise / LOW = clockwise)
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(800);                                                                                 // 800 microseconds seems to be the smallest reliable value
    digitalWrite(stepPin, LOW);
    delay(b);
  }
}

/**********************************************************************************************************************************************************************************************************************************
  As the RunBack function with the differences that it runs the stepper motor forwards and there is no need to check for an end-stop pin.   Parameter passed sets the speed.
***********************************************************************************************************************************************************************************************************************************/
void RunForward(int c) {
  digitalWrite(enablePin, LOW);                                                                             // Wake stepper
  digitalWrite(dirPin, LOW);                                                                                // (HIGH = anti-clockwise / LOW = clockwise)
  digitalWrite(stepPin, HIGH);
  delayMicroseconds(800);                                                                                   // 800 microseconds seems to be the smallest reliable value 
  digitalWrite(stepPin, LOW);
  delay(c);
}

/**********************************************************************************************************************************************************************************************************************************
  Here we simply return to the 'IN' point when called.   The stepper is awakened.   We check that both the 'IN' and 'OUT' points were set and from this calculate the number of steps we need to reverse back to the 'IN' point.
  Given that the stepper could not move after these points were initially set we know that the reverse step count is accurate.   We need to account for the backlash which is different for each microstep size.
  This level of backlash is identified as intBacklash.   When we call ReturnIn() we increment the forwardSteps by this amount so that the loop used by RunBack() will take up the backlash before running back the exact number
  of steps to the IN point.   Once at the IN point we have to take up the backlash on the leadscrew once again in preparation for a Forward run.  After the return to 'IN' we put the stepper back to 'Sleep'.
***********************************************************************************************************************************************************************************************************************************/
void returnIn() {
  digitalWrite(enablePin, LOW);                                                                             // Wake stepper
  if (stepsForward > 0) {                                                                                   // Only accept button press if we have moved Forwards
    long inTemp = in;
    long stepsForwardTemp = (stepsForward + intBacklash);                                                   // Increment stepsForward count to allow return to IN point allowing for backlash
    if (inSet == true  && outSet == true) {                                                                 // You can only return if IN and OUT have actully been set 
      while (stepsForwardTemp != inTemp ) {                                                                 // Step back to the original starting point i.e. the IN point
        RunBack(10);
        stepsForwardTemp--;
      }
    }
  }
  for (r = 0; r < intBacklash; r++) {                                                                       // Take up the backlash from the reverse move back to IN
    RunForward(10);
  }
  r = 0;                                                                                                    // Reset r to zero
  previousDirection = 1;                                                                                    // Last stepper movement was Forwards
  runSet = true;
  digitalWrite(enablePin, HIGH);                                                                            // Switch off stepper motor
}
