/**********************************************************************************************************************************************************************************************************************************
  Here we fire the shutter by setting the shutter pin HIGH for 1 millisecond.  This the allows the opto-isolator to trigger the camera.   If we have set the variable fireShutter to 1 the camera flash will trigger and the Run
  will be delayed to give the flash unit time to recharge.
***********************************************************************************************************************************************************************************************************************************/
void ShutterFire() {
  if (fireShutter == 0) {                                                                                   // Default is 'Live' run
    delay(flashDelay);                                                                                      // Delay to allow flash to recharge and vibrations to stop
    digitalWrite(shutterPin, HIGH);                                                                         // We fire the shutter once each loop
    delay(500);                                                                                             // Delay to ensure shutter fired / flash triggered
    digitalWrite(shutterPin, LOW);
  }
}
