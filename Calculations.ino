/**********************************************************************************************************************************************************************************************************************************
  The user has selected the Sensor size depending on the camera in use.   The user then has to physically measure the horizontal size of the frame in view and input this value to a field in the GUI..   The Circle Of Confusion
  is fixed depending on the Sensor but the Magnification and the Effective fStop values can now be calculated.   The returned values for coC and Magnification are required to calculate Depth of Field.
***********************************************************************************************************************************************************************************************************************************/
void magnificationCalc()
{
  if (sensor == 0)                                                                                          // 0 is Full Frame
  {
    magnification = 36.00 / frameSize;                                                                      // Magnification depends on field of view in viewfinder, measured by target
    coC = 0.030;                                                                                            // Circle of Confusion - See http://www.dofmaster.com/digital_coc.html
    effectivefStop = (fStop * (magnification + 1));                                                         // Effective fStop   = fStop * (1 + magnification)
  }

  if (sensor == 1)                                                                                          // 1 is Canon APS-H e.g. Canon 1D MkIV
  {
    magnification = 27.90 / frameSize;
    coC = 0.023;
    effectivefStop = (fStop * (magnification + 1));
  }
  if (sensor == 2)                                                                                          // 2 is APS-C e.g. Nikon DX
  {
    magnification = 23.60 / frameSize;
    coC = 0.020;
    effectivefStop = (fStop * (magnification + 1));
  }
  if (sensor == 3)                                                                                          // 3 is Canon APS-C
  {
    magnification = 22.20 / frameSize;
    coC = 0.019;
    effectivefStop = (fStop * (magnification + 1));
  }
  else if (sensor == 4)                                                                                     // 4 is 1.5" sensor e.g. Canon G1X
  {
    magnification = 18.70 / frameSize;
    coC = 0.015;
    effectivefStop = (fStop * (magnification + 1));
  }
  else if (sensor == 5)                                                                                     // 5 is micro four thirds sensor e.g. Panasonic G9
  {
    magnification = 17.30 / frameSize;
    coC = 0.015;
    effectivefStop = (fStop * (magnification + 1));
  }
  else if (sensor == 6)                                                                                     // 6 is 1.0" sensor
  {
    magnification = 12.80 / frameSize;
    coC = 0.011;
    effectivefStop = (fStop * (magnification + 1));
  }

}

/**********************************************************************************************************************************************************************************************************************************
  The Depth Of Field is the extent of an acceptable range of sharpness achieved in an image based on the fStop used and the level of Magnification of the subject matter.    Given this level of acceptable sharpness will be very
  small it will be necessary to take multiple images at different points of focus throughout the 'depth' of the target subject.   To ensure sharpness we will also have to allow a margin of overlap - in this calculation we have
  chosen a 20% safety margin i.e. the calculated depth of field is finally reduced by 20%.
***********************************************************************************************************************************************************************************************************************************/
void dofCalc( )
{
  dof = ((2 * coC * fStop * 1000.00) * (magnification + 1)) / (magnification * magnification);              // 2cn(m+1)/m*m c=coC, n=fstop, m=magnification.
  dof = dof * 0.80;                                                                                         // Reduction of 20% as a safety margin when
}                                                                                                           // stacking images

/**********************************************************************************************************************************************************************************************************************************
  We will have obtained through counting the steps between the IN and OUT points of the stepper position when physically moving the camera to establish the closest and furthest points of focus we require based on the target.
  This will give us a value for the total number of steps to be taken.   We also have a value for the Depth Of Field so we now know how many steps we will need to take to reach the next point where an image should be taken and
  will be within the range of sharpness.   This figure will allow us to 'loop' though this number of steps pausing to allow any vibrations to subside before firing the shutter, pause, then continue until the total distance has
  been travelled and a final image taken.
***********************************************************************************************************************************************************************************************************************************/
void stepsBetweenShutters( )                                                                                // This is the number of steps in each loop
{
  stepCountDouble = (double) stepCount;
  roughLoopsDouble = ((stepCountDouble * stepLength) / dof);                                                // An accurate distance divided by dof
  stepsPerLoop = (stepCountDouble / roughLoopsDouble);                                                      // An integer value
  if (roughLoopsDouble < 1.00) {
    roughLoops = 1;
    stepsPerLoop = stepCount;
  }
  else {
    roughLoops = int(roughLoopsDouble);
  } 
  roughTotal = (stepsPerLoop * roughLoops);
  remainder = (stepCount - roughTotal);                                                                     // Calculate remaining steps 'lost' due to int
  addToLoops = (remainder / stepsPerLoop);
  loops = roughLoops + addToLoops;
  if (remainder > 0) {
    shutters = loops + 2;
  }
  else {
    shutters = (loops + 1);                                                                                 // 1 before loop starts + 1 for residue + loops
  }
}
/***********************************************************************************************************************************************************************************************************************************
  We need to calculate exactly how far each single step will move the linear rail.   This will depend on the hardware and electronics employed.  The default for the linear rail is 1mm per revolution of the leadscrew, 200 steps
  of the stepper motor per revolution.  To get the maximum accuracy I default the EasyDriver hardware to microsteps of 1/16th.  The default value then being 200 * 16 = 3200 steps per millimetre of travel i.e 0.0003125mm per step.
  This is too small for the mathematical precision of the Arduino Uno or Mega - so 1000 steps used in the calculations.  To couteract this we have multiplied the Depth Of Field calculation by the same amount - thus counteracting
  and ensuring an accurate end result.
************************************************************************************************************************************************************************************************************************************/
void stepSize()
{
  stepLength = ((leadscrew / stepper * 1000.00 ) * (microStepSize));                                       // the length of 1,000 steps
}
