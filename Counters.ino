/**********************************************************************************************************************************************************************************************************************************
  Increments the Distance to travel in increments of 1/10th millimetre
***********************************************************************************************************************************************************************************************************************************/
void distanceCounterUp() {
  if (inSet == true && outSet == false) {
    tempDistance++;
    distance = tempDistance / 10;                                                                              // Distance to travel in 10ths of mm
    tft.fillRoundRect(380, 172, 90, 30, 4, CYAN);                                                              // Distance field cleared ready for new value
    tft.setCursor(395, 180);
    tft.print(distance);
  }
}

/**********************************************************************************************************************************************************************************************************************************
  Decrements the Distance to travel in increments of 1/10th millimetre
***********************************************************************************************************************************************************************************************************************************/
void distanceCounterDown() {
  if (inSet == true && outSet == false && tempDistance > 0) {                                                  // Ensure we cannot input a negative Distance
    tempDistance--;
    distance = tempDistance / 10;                                                                              // Distance to travel in 10ths of mm
    tft.fillRoundRect(380, 172, 90, 30, 4, CYAN);                                                              // Distance field cleared ready for new value
    tft.setCursor(395, 180);
    tft.print(distance);
  }
}

/**********************************************************************************************************************************************************************************************************************************
 Increments the Framesize i.e size of the image frame as seen through the camera viewfinder.   This is achieved by placing a ruler behind the target image and assessing the horizontal width of the total image.
 The frame size is incremented / decremented in steps of 1/10th of a millimetre.   The default is setup as 10.00 mm to ensure a minimum value and therefore protect against a failure to input a valid amount required in Screen 2.
 It is not possible to move forwards from the Input screen until framesize is changed.
***********************************************************************************************************************************************************************************************************************************/
void frameCounterUp() {
  tft.fillRoundRect(139, 188, 75, 30, 4, CYAN);                                                               // Clear framesize
  y++;
  frameSize = (y / 10);                                                                                       // Framesize in 1/10th of a millimetre
  tft.setCursor(150, 195);
  tft.setTextColor(BLACK);
  tft.print(frameSize);                                                                                       // Setup default 100.00 therefore starts at 10.00 mm
  delay(100);
}

/**********************************************************************************************************************************************************************************************************************************
  Decrements the Framesize i.e size of the image frame as seen through the camera viewfinder.   This is achieved by placing a ruler behind the target image and assessing the horizontal width of the total image.
  The frame size is incremented / decremented in steps of 1/10th of a millimetre.   The default is setup as 10.00 mm to ensure a minimum value and therefore protect against a failure to input a valid amount required in Screen 2.
***********************************************************************************************************************************************************************************************************************************/
void frameCounterDown() {
  if (y > 1.00) {                                                                                             // We can decrement to 0.10 mm
    tft.fillRoundRect(139, 188, 75, 30, 4, CYAN);
    y--;
    frameSize = (y / 10);
    tft.setCursor(150, 195);
    tft.setTextColor(BLACK);
    tft.print(frameSize);      
    delay(100);
  }
}
