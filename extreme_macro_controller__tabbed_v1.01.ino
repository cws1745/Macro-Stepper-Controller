/*********************************************************************************************************************************************************************************************************************************
  Extreme Macro controller V1.0 17th June 2019.   Code written by C.W Shearer.
  Based on Arduino Mega or Arduino DUE with Big EasyDiver, GUI presented on 3.5" TFT Touchscreen.
*********************************************************************************************************************************************************************************************************************************/
#include <Adafruit_GFX.h>                                                                           // See: https://github.com/adafruit/Adafruit-GFX-Library
#include <MCUFRIEND_kbv.h>                                                                          // See: https://github.com/prenticedavid/MCUFRIEND_kbv
MCUFRIEND_kbv tft;
#include <TouchScreen.h>
#define MINPRESSURE 200                                                                             // Maximum and minimum pressures to read from touchscreen
#define MAXPRESSURE 1000
const int XP = 8, XM = A2, YP = A3, YM = 9;                                                         // 320x480 ID=0x9486 RS, WR, CS, RST

/**********************************************************************************************************************************************************************************************************************************
  All TFT screens need to be calibrated before accurate identification of the touchpoints.   There are a number of calibration sketches, hopefully supplied with the screen.   For the screen used the MCUFRIEND.kbv Examples
  folder contains the sketch Touchscreen_calbr_native.   The information provided by the calibration sketch is then cut 'n pasted to the code below and the screen orientation mode Landscape / Portrait selected.
 *********************************************************************************************************************************************************************************************************************************/
//const int TS_LEFT = 932, TS_RT = 141, TS_TOP = 829, TS_BOT = 167;                                 // Landscape for DUE
const int TS_LEFT = 950, TS_RT = 94, TS_TOP = 898, TS_BOT = 120;                                    // Landscape for MEGA

TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);                                                  // For precise results measure resistance and amend final figure e.g. 322
int pixel_x, pixel_y;
bool Touch_getXY(void)                                                                              // Touch_getXY() updates global vars

{
  TSPoint p = ts.getPoint();
  pinMode(YP, OUTPUT);                                                                              // Restore shared pins
  pinMode(XM, OUTPUT);
  bool pressed = (p.z > MINPRESSURE && p.z < MAXPRESSURE);
  if (pressed) {
    pixel_x = map(p.y, TS_LEFT, TS_RT, 0, tft.width());
    pixel_y = map(p.x, TS_TOP, TS_BOT, 0, tft.height());
  }
  return pressed;
}

/**********************************************************************************************************************************************************************************************************************************
  BUTTONS.   Need to be listed for each screen in which they are to appear.   Some 'long' variables required as in 1/16 microsteps the values exceed that of integer limits!
 *********************************************************************************************************************************************************************************************************************************/
Adafruit_GFX_Button userInput_btn, runSetup_btn, runControl_btn, back_btn, lens_btn, sensor_btn, fstop_btn, leadscrew_btn, stepper_btn, microstep_btn, liveOrTest_btn, stepDelay_btn, flashDelay_btn, accept_btn ;
Adafruit_GFX_Button reverse_btn, forward_btn, setIn_btn, setOut_btn, next_btn, countUp_btn, countDown_btn, jogReverse_btn, jogForward_btn, return_btn, reset_btn, run_btn, toggle_btn, full_btn, half_btn, quarter_btn, eighth_btn ;
Adafruit_GFX_Button sixteenth_btn, runBack_btn, main_btn ;
Adafruit_GFX_Button *screen0[] = {&userInput_btn, &runSetup_btn, &runControl_btn, NULL };
Adafruit_GFX_Button *screen1[] = {&lens_btn, &sensor_btn, &fstop_btn, &leadscrew_btn, &stepper_btn, &microstep_btn, &liveOrTest_btn, &stepDelay_btn, &flashDelay_btn, &back_btn, &countUp_btn, &countDown_btn, &accept_btn, NULL };
Adafruit_GFX_Button *screen2[] = {&back_btn, &reverse_btn, &setIn_btn, &setOut_btn, &forward_btn, &next_btn, &jogReverse_btn, &jogForward_btn, &return_btn, &run_btn, &reset_btn, &toggle_btn, NULL };
Adafruit_GFX_Button *screen3[] = {&main_btn, &full_btn, &half_btn, &quarter_btn, &eighth_btn, &sixteenth_btn, &runBack_btn, NULL };

#define endStopPin 41                                                                               // Pin 22 an end stop for 'Home' position.   Reads HIGH when not closed.
#define shutterPin 39                                                                               // Connection to camera via opto-isolator
#define enablePin 43                                                                                // Use Enable pin to put stepper to sleep.   More accurate than using Sleep which causes 'jitter'
#define dirPin 45                                                                                   // Used to dictate forward or reverse stepper motor
#define stepPin 47                                                                                  // Any transition on this pin from LOW to HIGH will trigger the motor to step forward one step
#define ms1 49                                                                                      // Used to control step size i.e. microstepping
#define ms2 51                                                                                      // See: https://learn.sparkfun.com/tutorials/big-easy-driver-hookup-guide/all
#define ms3 53                                                                                      // Defaulting to Full Steps
int jogSpeed = 0;                                                                                   // To control speed of counter increment for Jog buttons
int intBacklash = 16;                                                                               // Default for Full Steps 
int previousDirection = 1;                                                                          // Used to record last direction travelled.   Needed for backlash calculations
int loops = 0;                                                                                      // The number of loops created as a result of Depth of Field calculations
int stepsPerLoop = 0;                                                                               // The number of steps in each loop before each shutter actuation
long stepCount = 0;                                                                                 // Calculates the net forward steps made i.e. the run length
long stepsForward = 0;                                                                              // The number of steps forward
long stepsBack = 0;                                                                                 // The number of steps back
int stepLoop = 0;                                                                                   // The number of times we loop through steps and shutter actuations
int flashDelay = 5000;                                                                              // After each shutter / flash discharge a delay for recharge / vibration to stop.  Default 5 seconds
int stepDelay = 2000;                                                                               // Delay before shutter fired - to allow any vibrations to subside.   ms as default
int fireShutter = 0;                                                                                // This is setting up a Test or a 'live' run i.e. take images or not.  Default 'live' run
double stepCountDouble = 0.00;                                                                      // Used in distance calculations.
double roughLoopsDouble = 0.00;                                                                     // Used in calculation of steps between shutters
double magnification = 0.00;                                                                        // Calculated value - required in Depth of Field calculations
double dof = 0.00;                                                                                  // Calculated value - dictates the number of steps before each shutter actuation
double effectivefStop = 0.00;                                                                       // Calculated value.   Identifies 'real' fstop and warns of diffraction / loss of sharpness
double coC = 0.00;                                                                                  // Circle of Confusion.  Established value.   See: http://www.dofmaster.com/digital_coc.html
int shutters = 0;                                                                                   // Number of images to be taken.  DSLR triggered via opto-isolator
int tempShutters = 0;                                                                               // Used in PCRun to restore the shutter count after a Run and allow a repeat of the Run
int sensor = 3;                                                                                     // Default value i.e. APS-C
double stepLength = 0.00;                                                                           // The calculated length of each step.  Depends on leadscrew, stepper angle and microsteps
double fStop = 2.80;                                                                                // Default value
double microStepSize = 1.00;                                                                        // Default value of one full step
double leadscrew = 1.00;                                                                            // Default value of 1 mm pitch
double stepper = 200.00;                                                                            // Default value of 1.8 degree stepper angle i.e. 200 steps per revolution
double y = 100.00;                                                                                  // Used in FrameCounter methods.   First divided by 10 hence 100.00 gives 10mm starting value
double distance = 0.00;                                                                             // Total travel between IN and OUT points - calculated or manually input
double tempDistance = 0.00;                                                                         // Used in Distance counter.  Distance variable actually 1/10th millimeter
boolean runSet = false;                                                                             // Used to control stepper i.e. disallow movements after run is set
boolean inSet = false;                                                                              // Used in IN method and in Reverse, Forward and Jog methods
boolean outSet = false;                                                                             // Used in OUT method and in Reverse, Forward and Jog methods
boolean stepControl = true;                                                                         // Used to control when Reverse, Forward and Jog methods run stepper or Distance counts
boolean distanceControl = false;                                                                    // Used to control when Reverse, Forward and Jog methods run stepper or Distance counts
int roughLoops = 0;                                                                                 // Used in Calculation of Loops
int roughTotal = 0;                                                                                 // Used in Calculation of Loops
int addToLoops = 0;                                                                                 // Used in Calculation of Loops
int remainder = 0;                                                                                  // Used in Calculation of Loops
long tempCount = 0;                                                                                 // Used in PCRun()
long stepsTaken = 0;                                                                                // Used in PCRun()
long totalStepsTaken = 0;                                                                           // Used in setOut_btn
long stepCalc = 0;                                                                                  // Used in calculation of OUT point
long in = 0;                                                                                        // Default value
long out = 0;                                                                                       // Default value
int screen = 0;                                                                                     // The opening screen
int lens = 0;                                                                                       // Default value to dictate type of lens used
double frameSize = 0.50;                                                                            // The size of the horizontal plane in the camera viewfinder.   Needed in Magnificationand DOF calculations
int j = 3;                                                                                          // Default for sensor is APS-C
int a, b, c, i, k, l, m, n, o, p, q, r, s, x = 0;                                                   // Loop variables
int z = 600;                                                                                        // Full steps to clear microswitch at Startup or Reset
char *lensString[] = {"MPE65", "A", "B", "OBJ"};                                                    // Array for types of lens used. First item in array is default
char *sensorString[] = {"Full", "APS-H", "APS-CN", "APS-C", "1.5", "4/3", "1.0"};                   // Array for camera sensor types.First item in array is default
char *fstopString[] = {"2.8", "3.5", "4.0", "5.6", "6.3", "8.0", "11", "16", "1.2", "1.4"};         // Array for lens apertures. First item in array is default
char *leadscrewString[] = {"1.0", "2.0", "4.0", "8.0"};                                             // Array for leadscrew pitches. First item in array is default
char *stepperString[] = {"200", "400"};                                                             // Array for stepper types i.e. 200 steps per revoliion. First item in array is default
char *microstepString[] = {"Full", "1/2", "1/4", "1/8", "1/16"};                                    // Array for microsteps supported by Big Easydriver. First item in array is default
char *liveOrTestString[] = {"Live", "Test"};                                                        // Array for Live run with shutter actuations or Test run with no images taken. First item in array is default
char *stepDelayString[] = {"2secs", "2.5", "3secs", "3.5", "4secs", "4.5", "5secs", "5.5", "6secs"};   // Array for Delays (in milliseconds) used in stepper runs - effectively sets stepper speed
char *flashDelayString[] = {"5", "6", "7", "8", "9", "1", "2", "3", "4"};                           // Array for delays (in seconds) to allow falsh to recharge after each shutter actuation

#define BLACK   0x0000                                                                              // User friendly definitions of colours used
#define BLUE    0x001F                                                                              // User friendly definitions of colours used
#define RED     0xF800                                                                              // User friendly definitions of colours used
#define GREEN   0x07E0                                                                              // User friendly definitions of colours used
#define CYAN    0x07FF                                                                              // User friendly definitions of colours used
#define MAGENTA 0xF81F                                                                              // User friendly definitions of colours used
#define YELLOW  0xFFE0                                                                              // User friendly definitions of colours used
#define WHITE   0xFFFF                                                                              // User friendly definitions of colours used

void drawHome()                                                                                     // The opening screen, first display
{
  tft.fillScreen(BLUE);                                                                             // Blue screen background
  tft.drawRect(0, 0, 480, 320, WHITE);                                                              // Page border of a fine white line
  tft.setCursor(155, 20);                                                                           // In format (x-axis value, y-axis value)
  tft.setTextSize(2);
  tft.setFont();
  tft.setTextColor(WHITE);
  tft.print("SELECT MENU");
  tft.setCursor(122, 55);
  tft.setTextSize(2);
  tft.setTextColor(YELLOW);
  tft.print("C.W.Shearer V1.0 170619");
  tft.setTextColor(WHITE);
}

void setup()
{
  Serial.begin(9600);                                                                               // Needed for testing i.e. use of the Serial Monitor
  pinMode(endStopPin, INPUT_PULLUP);                                                                // An Input as the pin reads the value from the endstop switch + or - 5v
  pinMode(shutterPin, OUTPUT);                                                                      // A High of 5v triggers an opto-isolator and therefore the camera shutter
  pinMode(ms1, OUTPUT);                                                                             // See: https://learn.sparkfun.com/tutorials/big-easy-driver-hookup-guide/all
  pinMode(ms2, OUTPUT);
  pinMode(ms3, OUTPUT);
  digitalWrite(shutterPin, LOW);
  digitalWrite(ms1, LOW);                                                                           // Set Microstepping to a default of Full Steps
  digitalWrite(ms2, LOW);
  digitalWrite(ms3, LOW);
  pinMode(enablePin, OUTPUT);
  pinMode(dirPin, OUTPUT);
  pinMode(stepPin, OUTPUT);
  digitalWrite(enablePin, HIGH);                                                                    // Sets power down as default for power / heat savings
  uint16_t identifier = tft.readID();                                                               // Identify TFT screen controller
  if (identifier == 0xEFEF) identifier = 0x9486;                                                    // Commonly used controller is ILI9486
  tft.begin(identifier);
  tft.fillScreen(BLUE);
  tft.setRotation(1);                                                                               // 0 = Portrait, 1 = Landscape
  tft.setTextSize(3);
  tft.setTextColor(YELLOW);
  tft.drawRect(0, 0, 480, 320, WHITE);                                                              //Page border
  tft.setCursor(25, 40);
  tft.print("Loading .....");
  tft.setCursor(25, 90);
  tft.print("Extreme Macro Controller");
  tft.setCursor(25, 140);
  tft.print("Version 1.0 170619");
  tft.setCursor(25, 190);
  tft.print("C.W. Shearer .. 2019");
  for (int i=0; i < 350; i++) {                                                                     // i Dictates time and length of bar
    tft.fillRect(20, 250, i, 20, RED);
  }
  startUp();
  drawHome();

  /**********************************************************************************************************************************************************************************************************************************
    BUTTONS. Initiated for all screens.   Format (x-axis, y-axis, wWidth, Height, Border, Colour, Colour when pressed, Label, Corner radius).   I am using them to display array values in many instances.
   *********************************************************************************************************************************************************************************************************************************/

  runControl_btn.initButton(&tft, 220, 240, 240, 45, WHITE, RED, BLACK, "UTILITIES", 2);
  runSetup_btn.initButton(&tft,  220, 180, 240, 45, WHITE, RED, BLACK, " SETUP", 2);
  userInput_btn.initButton(&tft,  220, 120, 240, 45, WHITE, RED, BLACK, "INPUTS", 2);
  back_btn.initButton(&tft,  50, 20, 80, 30, WHITE, RED, BLACK, "BACK", 2);
  lens_btn.initButton(&tft, 175, 58, 75, 30, WHITE, CYAN, BLACK, lensString[i], 2);
  sensor_btn.initButton(&tft, 175, 106, 75, 30, WHITE, CYAN, BLACK, sensorString[j], 2);
  fstop_btn.initButton(&tft, 175, 154, 75, 30, WHITE, CYAN, BLACK, fstopString[k], 2);
  leadscrew_btn.initButton(&tft, 175, 250, 75, 30, WHITE, CYAN, BLACK, leadscrewString[l], 2);
  stepper_btn.initButton(&tft, 425, 58, 75, 30, WHITE, CYAN, BLACK, stepperString[m], 2);
  microstep_btn.initButton(&tft, 425, 106, 75, 30, WHITE, CYAN, BLACK, microstepString[n], 2);
  liveOrTest_btn.initButton(&tft, 425, 154, 75, 30, WHITE, CYAN, BLACK, liveOrTestString[o], 2);
  stepDelay_btn.initButton(&tft, 425, 202, 75, 30, WHITE, CYAN, BLACK, stepDelayString[p], 2);
  flashDelay_btn.initButton(&tft, 425, 250, 75, 30, WHITE, CYAN, BLACK, flashDelayString[q], 2);
  accept_btn.initButton(&tft, 425, 20, 80, 30, WHITE, RED, BLACK, "ACCEPT", 2);
  return_btn.initButton(&tft, 40, 290, 70, 30, WHITE, RED, BLACK, "HOME", 2);
  jogReverse_btn.initButton(&tft, 120, 290, 70, 30, WHITE, RED, BLACK, "< JOG", 2);
  reverse_btn.initButton(&tft, 200, 290, 70, 30, WHITE, RED, BLACK, "<<<", 2);
  toggle_btn.initButton(&tft, 360, 250, 70, 30, WHITE, RED, BLACK, "STEPS", 2);
  setIn_btn.initButton(&tft, 40, 250, 70, 30, WHITE, RED, BLACK, "IN", 2);
  setOut_btn.initButton(&tft, 440, 250, 70, 30, WHITE, RED, BLACK, "OUT", 2);
  jogForward_btn.initButton(&tft, 360, 290, 70, 30, WHITE, RED, BLACK, "JOG >", 2);
  reset_btn.initButton(&tft, 440, 290, 70, 30, WHITE, RED, BLACK, "RESET", 2);
  forward_btn.initButton(&tft, 280, 290, 70, 30, WHITE, RED, BLACK, ">>>", 2);
  next_btn.initButton(&tft,  425, 20, 80, 30, WHITE, RED, BLACK, "NEXT", 2);
  countUp_btn.initButton(&tft, 120, 295, 80, 30, WHITE, RED, BLACK, "UP", 2);
  countDown_btn.initButton(&tft, 210, 295, 80, 30, WHITE, RED, BLACK, "DOWN", 2);
  run_btn.initButton(&tft, 120, 250, 70, 30, WHITE, RED, BLACK, "RUN", 2);
  full_btn.initButton(&tft, 40, 290, 70, 30, WHITE, RED, BLACK, "FULL", 2);
  half_btn.initButton(&tft, 120, 290, 70, 30, WHITE, RED, BLACK, "HALF", 2);
  quarter_btn.initButton(&tft, 200, 290, 70, 30, WHITE, RED, BLACK, "1/4", 2);
  eighth_btn.initButton(&tft, 280, 290, 70, 30, WHITE, RED, BLACK, "1/8", 2);
  sixteenth_btn.initButton(&tft, 360, 290, 70, 30, WHITE, RED, BLACK, "1/16", 2);
  runBack_btn.initButton(&tft, 440, 290, 70, 30, WHITE, RED, BLACK, "RTN", 2);
  main_btn.initButton(&tft,  50, 20, 80, 30, WHITE, RED, BLACK, "START", 2);
  userInput_btn.drawButton(false);
  runSetup_btn.drawButton(false);
  runControl_btn.drawButton(false);
}

/***********************************************************************************************************************************************************************************************************************************
    Within the main loop we now have all of the Button controls and the functions they perform.  For an insight into how the buttons work see the 'button_simple' sketch in the Examples folder for the MCUFRIEND.kbv library.
 **********************************************************************************************************************************************************************************************************************************/
void loop()
{
  if (screen == 0) {
    update_button_list(screen0);                                                                    // Process all buttons
    if (userInput_btn.justPressed()) {
      screen = 1;                                                                                   // Input screen for Run variables
      draw_button_list(screen1);
    }
    if (runSetup_btn.justPressed()) {                                                               // Switch to Run Controls, Set IN and OUT points
      screen = 2;
      draw_button_list(screen2);
    }
    if (runControl_btn.justPressed()) {                                                             // Switch to Screen 3
      screen = 3;
      draw_button_list(screen3);
    }
  }

  if (screen == 1) {
    update_button_list(screen1);                                                                    // Process all buttons

    if (lens_btn.justPressed()) {                                                                   // Select type of lens used
      lens_btn.drawButton(true);
      i++;
      if (i > 3) {
        i = 0;
      }
      lens_btn.initButton(&tft, 175, 58, 75, 30, WHITE, CYAN, BLACK, lensString[i], 2);
      switch (i) {
      }
    }
    if (sensor_btn.justPressed()) {                                                                 // Select Sensor used in camera
      sensor_btn.drawButton(true);
      j++;
      if (j > 6) {                                                                                  // The limit for Sensor Types
        j = 0;
      }
      sensor_btn.initButton(&tft, 175, 106, 75, 30, WHITE, CYAN, BLACK, sensorString[j], 2);
      sensor = j;                                                                                   // See magnificationCalc()
    }
    if (fstop_btn.justPressed()) {
      fstop_btn.drawButton(true);
      k++;
      if (k > 9) {                                                                                  // The limit for fStop values
        k = 0;
      }
      fstop_btn.initButton(&tft, 175, 154, 75, 30, WHITE, CYAN, BLACK, fstopString[k], 2);
      switch (k) {
        case 0:
          fStop = 2.80;
          break;
        case 1:
          fStop = 3.50;
          break;
        case 2:
          fStop = 4.00;
          break;
        case 3:
          fStop = 5.60;
          break;
        case 4:
          fStop = 6.30;
          break;
        case 5:
          fStop = 8.00;
          break;
        case 6:
          fStop = 11.00;
          break;
        case 7:
          fStop = 16.00;
          break;
        case 8:
          fStop = 1.20;
          break;
        case 9:
          fStop = 1.40;
          break;
      }
    }

    if (leadscrew_btn.justPressed()) {
      leadscrew_btn.drawButton(true);
      l++;
      if (l > 3) {                                                                                    // The limit for Leadscrew values
        l = 0;
      }
      leadscrew_btn.initButton(&tft, 175, 250, 75, 30, WHITE, CYAN, BLACK, leadscrewString[l], 2);
      switch (l) {
        case 0:
          leadscrew = 1.00;
          break;
        case 1:
          leadscrew = 2.00;
          break;
        case 2:
          leadscrew = 4.00;
        case 3:
          leadscrew = 8.00;
          break;
      }

    }
    if (stepper_btn.justPressed()) {
      stepper_btn.drawButton(true);
      m++;
      if (m > 1) {                                                                                     // The limit for Stepper motor values (steps per rev.)
        m = 0;
      }
      stepper_btn.initButton(&tft, 425, 58, 75, 30, WHITE, CYAN, BLACK, stepperString[m], 2);
      switch (m) {
        case 0:
          stepper = 200;
          break;
        case 1:
          stepper = 400;
          break;
      }
    }
    if (microstep_btn.justPressed()) {
      n++;
      if (n > 4) {
        n = 0;
      }
      microstep_btn.drawButton(true);
      microstep_btn.initButton(&tft, 425, 106, 75, 30, WHITE, CYAN, BLACK, microstepString[n], 2);
      switch (n)                                                                                        // Sets MS1, MS2, MS3 i.e. the Microstepping of the BigEasydriver board
      {
        case 0:
          digitalWrite (ms1, LOW);                                                                      // Full steps is the default
          digitalWrite (ms2, LOW);
          digitalWrite (ms3, LOW);
          microStepSize = 1.00;
          intBacklash = 16;                                                                             // Backlash calculated in step size 
          z = 600;                                                                                      // Used in runReset()
          break;
        case 1:
          digitalWrite (ms1, HIGH);                                                                     // Half steps
          digitalWrite (ms2, LOW);
          digitalWrite (ms3, LOW);
          microStepSize = 0.50;
          intBacklash = 33;
          z = 1200;
          break;
        case 2:
          digitalWrite (ms1, LOW);                                                                      // Quarter steps
          digitalWrite (ms2, HIGH);
          digitalWrite (ms3, LOW);
          microStepSize = 0.25;
          intBacklash = 66;
          z = 2400;
          break;
        case 3:
          digitalWrite (ms1, HIGH);                                                                     // Eighth steps
          digitalWrite (ms2, HIGH);
          digitalWrite (ms3, LOW);
          microStepSize = 0.125;
          intBacklash = 132;
          z = 4800;
          break;
        case 4:
          digitalWrite (ms1, HIGH);                                                                     // Sixteenth steps for BigEasyDriver
          digitalWrite (ms2, HIGH);
          digitalWrite (ms3, HIGH);
          microStepSize = 0.0625;
          intBacklash = 262;
          z = 9600;
          break;
      }
    }
    if (liveOrTest_btn.justPressed()) {
      liveOrTest_btn.drawButton(true);
      o++;
      if (o > 1) {
        o = 0;
      }
      liveOrTest_btn.initButton(&tft, 425, 154, 75, 30, WHITE, CYAN, BLACK, liveOrTestString[o], 2);
      switch (o) {
        case 0:                                                                                           // The default is for Live runs i.e. Shutter will fire
          fireShutter = 0;                                                                                // Default i.e. 'Live' run
          break;
        case 1:
          fireShutter = 1;
          break;
      }
    }
    if (stepDelay_btn.justPressed()) {
      stepDelay_btn.drawButton(true);
      p++;
      if (p > 8) {                                                                                        // The limit for Step Delay values
        p = 0;
      }
      stepDelay_btn.initButton(&tft, 425, 202, 75, 30, WHITE, CYAN, BLACK, stepDelayString[p], 2);
      switch (p) {
        case 0:                                                                                           // The default is for 1000 milliseconds i.e. 2 seconds
          stepDelay = 2000;
          break;
        case 1:
          stepDelay = 2500;
          break;
        case 2:
          stepDelay = 3000;
          break;
        case 3:
          stepDelay = 3500;
          break;
        case 4:
          stepDelay = 4000;
          break;
        case 5:
          stepDelay = 4500;
          break;
        case 6:
          stepDelay = 5000;
          break;
        case 7:
          stepDelay = 5500;
          break;
        case 8:
          stepDelay = 6000;
          break;
      }
    }
    if (flashDelay_btn.justPressed()) {
      flashDelay_btn.drawButton(true);
      q++;
      if (q > 8) {                                                                                        // The limit for Flash Delay (Recharge time) values
        q = 0;
      }
      flashDelay_btn.initButton(&tft, 425, 250, 75, 30, WHITE, CYAN, BLACK, flashDelayString[q], 2);
      switch (q) {
        case 0:                                                                                          // The default is for 5000 milliseconds i.e. 5 seconds
          flashDelay = 5000;
          break;
        case 1:
          flashDelay = 6000;
          break;
        case 2:
          flashDelay = 7000;
          break;
        case 3:
          flashDelay = 8000;
          break;
        case 4:
          flashDelay = 9000;
          break;
        case 5:
          flashDelay = 1000;
          break;
        case 6:
          flashDelay = 2000;
          break;
        case 7:
          flashDelay = 3000;
          break;
        case 8:
          flashDelay = 4000;
          break;
      }
    }

    if (countUp_btn.isPressed()) {                                                                         // No upper limit set - in 1/10th millimetres
      frameCounterUp();                                                                                    // Used to set Framesize
      tft.fillRoundRect(139, 188, 75, 30, 4, CYAN);                                                        // To clear previous screen and prevent values being continually overwritten
      tft.setCursor(150, 195);
      tft.setTextColor(BLACK);
      tft.print(frameSize);

    }
    if (countDown_btn.isPressed()) {
      if (frameSize > 0) {                                                                                 // Cannot be 0 or negative
        frameCounterDown();                                                                                // No limit set - in 1/10th millimetres
      }
    }
    if (accept_btn.justPressed()) {                                                                        // Used to Accept values selected or input, then call appropriate methods for calculations
      if (frameSize > 0.50) {                                                                              // No screen change until frameSize has been changed
        stepSize();
        magnificationCalc();
        dofCalc();
        screen = 2;
        draw_button_list(screen2);
        tft.setCursor(210, 230);
        tft.print("HOME");
      }
    }
    if (back_btn.justPressed()) {                                                                         // Back to the first Select Menu
      screen = 0;
      draw_button_list(screen0);
    }
  }

  /**********************************************************************************************************************************************************************************************************************************
    One of the first things we need to do is get details of the IN and OUT points of the stepper run.   To do this we employ an Arduino / BigEasydriver hardware combo. Touch buttons are used to move the stepper,  a continuous
    press for Forward / Reverse or single step JOG Forward or Reverse buttons as appropriate.   Touch Buttons also be used to 'set' the 'In' and 'Out' points for the live run.   OUT cannot be set before IN is set and OUT cannot
    be less than or equal to 'IN'.   When Microstepping the number of steps can be very large - so to alleviate the problem of having to hold down the Forward or Reverse buttons whilst the count increments it is possible to
    simply enter a value in tenths of a millimeter increments in the 'Distance' field.   The same Forward / Reverse and Jog buttons are used to increment / decrement the counter as in the 'IN' and 'OUT' points.   The Button
    default "STEP" toggles between "STEPS" and "DIST" to control this.   Once 'IN' and 'OUT' are set the nNumber of steps and Shutters in the Run are calculated and the stepper will return to the 'IN' position ready for an actual
    Run.  The Depth Of Field calculation is shown as the value for 1000 steps, this is necessary because of the lack of mathematical precision of the Arduino Mega, to ensure adequate precision the DOF value is multiplied by 1000
    as is the size of the steps e.g. 1/16thof a step is 0.0003125mm when a 1mm leadscrew is used!   The actual Magnification value is also shown as is the Effective Fstop.  The effective FStop needs to be considered as diffraction
    and therefore loss of sharpness can be an issue.   The "HOME" button will call the ReturnIn() method which will return the stepper to the IN position ready for another Run.  The "RESET" button will call runReset() which will
    clear the screen display of IN, OUT, Distance, Run Steps, Shutters and the relevant variables will be reset to zero.   The stepper will return until the end-stop is triggered.   The D.O.F., EfStop and Magnify values will
    remain untouched ready for new run variables to be supplied.
  **********************************************************************************************************************************************************************************************************************************/

  if (screen == 2) {
    update_button_list(screen2);                                                                            // Process all buttons
    tft.setTextColor(BLACK);
    tft.setCursor(395, 140);
    tft.print(dof);
    tft.setCursor(140, 140);
    tft.print(effectivefStop);
    tft.setCursor(140, 180);
    tft.print(magnification);

    /**********************************************************************************************************************************************************************************************************************************
      The toggle button works by simply switching the values and button lable each time it is pressed.   As a default it is set to display "STEPS" and therefore allow the use of the Forward, reverse and Jog buttons to increment or
      decrement the run steps anf therefore set the In and OUT points.   However, the button when pressed again will allow the same controls to increment the distance value (in 1/10th of a millimetre increments.
    ***********************************************************************************************************************************************************************************************************************************/

    if (toggle_btn.justPressed()) {                                                                         // Use the button as a toggle switch
      if (stepControl == true && distanceControl == false && outSet == false) {                             // But only if OUT is not already set
        stepControl = false;                                                                                // The button defaults to "Step"
        distanceControl = true;
        toggle_btn.initButton(&tft, 360, 250, 70, 30, WHITE, RED, BLACK, "DIST.", 2);
      }
      else if (stepControl == false && distanceControl == true) {
        stepControl = true;
        distanceControl = false;
        toggle_btn.initButton(&tft, 360, 250, 70, 30, WHITE, RED, BLACK, "STEPS", 2);
      }
    }

    /**********************************************************************************************************************************************************************************************************************************
      The Run and the stepper can only be triggered once the runSet value is 'true'.   The runSet variable also ensures that the stepper motors are disabled once the run is 'set'.   This prevents any changes to the stepper position
      and also ensures that other variables such as 'stepsForward' and 'stepsBack' cannot be changed.
    **********************************************************************************************************************************************************************************************************************************/
    if ((inSet == true) && (outSet == true)) {
      runSet = true;
    }

    /**********************************************************************************************************************************************************************************************************************************
      Simply sets the IN point if it is not already set.  Any back / forth movements required in finding the In point will have taken backlash into account and the steps required to take up the slack will not have been counted.
    ***********************************************************************************************************************************************************************************************************************************/
    if (setIn_btn.justPressed()) {
      if (inSet == false) {
        in = (stepsForward - stepsBack);                                                                    // We must always have moved overall forwards
        tft.fillRoundRect(380, 52, 90, 30, 4, CYAN);                                                        // If we haven't moved forwards at all 0 accepted as the initial point
        tft.setCursor(400, 60);
        tft.print(stepsForward - stepsBack);
        tft.fillRoundRect(168, 218, 144, 44, 4, CYAN);                                                      // Messagebox
        tft.setCursor(200, 230);
        tft.print("IN SET");
        inSet = true;
      }
    }
    /**********************************************************************************************************************************************************************************************************************************
      This routine is more complicated.   First of all we cannot set the 'OUT' point before the 'IN' point.   We also check that the 'OUT' point is not already set.  Given the pain of having to hold down the Run control buttons i.e
      Forward, Reverse buttons we can also add a Distance value - so we must check that this has not been done already i.e. distance == 0.  Assuming all of these conditions are met and the 'OUT' point is greater than the 'IN' point
      we can set the 'OUT' point.   The distance to travel, that is the total number of steps to be taken is calculated as simply the 'OUT' count minus the 'IN' count.   From this the Depth of Field calculation will work out how
      many steps can be taken within the required plane of focus and from that the number of loop and shutter actuations required.   The distance is also calculated and shown based on the calculated length of each step multiplied by
      the number of steps.   At the end of these calculations the stepper is moved back to the 'In' point ready for the Live or Test run.
     ***********************************************************************************************************************************************************************************************************************************/

    if (setOut_btn.justPressed()) {
      if (inSet == true && outSet == false && distance == 0) {                                              // In must be set already.   OUT not set before.
        out = (stepsForward - stepsBack);                                                                   // We must always have moved overall forwards.   Backlash catered for in RunForward() or RunBack()
        if (out > in) {                                                                                     // In and OUt cannot be the same.  OUT must be greater than IN
          tft.fillRoundRect(168, 218, 144, 44, 4, CYAN);
          tft.setCursor(195, 230);
          tft.print("RUN SET");
          outSet = true;
          tft.fillRoundRect(380, 92, 90, 30, 4, CYAN);
          tft.setCursor(400, 100);
          tft.print(out);
          stepCount = (out - in);                                                                           // stepCount is only valid steps i.e. any backlash steps are not counted
          tft.setCursor(150, 60);
          tft.fillRoundRect(130, 52, 90, 30, 4, CYAN);
          tft.print(stepCount);
          stepsBetweenShutters();                                                                           // Calculate the number of loops and therefor shutter actuations
          tft.setCursor(150, 100);
          tft.fillRoundRect(130, 92, 90, 30, 4, CYAN);
          tft.print(shutters);
          distance = ((stepCountDouble * stepLength ) / 1000.00 );
          tft.setCursor(395, 180);
          tft.fillRoundRect(380, 172, 90, 30, 4, CYAN);
          tft.print(distance);
          returnIn();                                                                                       // Go back to the start point for the Run
          stepsForward = in;
        }
      }
      else if (inSet == true && outSet == false && distance > 0) {                                          // Distance has been entered so we must use this value
        stepCalc = ((distance * 1000.00) / stepLength);
        out = (stepCalc + in);                                                                              // IN is the point from which we measure distance to OUT
        stepCount = (out - in);                    
        tft.fillRoundRect(168, 218, 144, 44, 4, CYAN);
        tft.setCursor(195, 230);
        tft.print("RUN SET");
        tft.fillRoundRect(380, 92, 90, 30, 4, CYAN);
        tft.setCursor(400, 100);
        tft.print(out);
        tft.setCursor(150, 60);
        tft.fillRoundRect(130, 52, 90, 30, 4, CYAN);
        tft.print(stepCount);
        stepsBetweenShutters();
        tft.fillRoundRect(130, 92, 90, 30, 4, CYAN);
        tft.setCursor(150, 100);
        tft.print(shutters);
        distance = ((stepCountDouble * stepLength) / 1000.00 );
        tft.setCursor(395, 180);
        tft.fillRoundRect(380, 172, 90, 30, 4, CYAN);
        tft.print(distance);
        returnIn();
        stepsForward = in;
      }
    }

    /**********************************************************************************************************************************************************************************************************************************
      Return to the 'IN' point ready for a Run.   The abort_btn will is effectively an emergency stop button.   Being linked to an interrupt pin, pressing this button will immediately trigger a return to the current 'IN' position
      but it will keep the variables already set 'unset' the OUT position thus allowing the Run to be reconfigured or Reset as deemed necessary.

    ***********************************************************************************************************************************************************************************************************************************/
    if (return_btn.justPressed()) {
      returnIn();                                                                                           // Back to the IN point ready for a Run.
    }

    /**********************************************************************************************************************************************************************************************************************************
      Reset the run variables and clear the input fields ready for fresh inputs.   Return the stepper to the point where the end-stop is triggered.   The DOF, Effective fStop and Magnification inputs remain as before.
    **********************************************************************************************************************************************************************************************************************************/
    if (reset_btn.justPressed()) {
      runReset();                                                                                           // Abandon Run setup, back to End stop, Variables cleared
    }

    /************************************************************************************************************************************************************************************************************************************
      The Forward button used to move the stepper to enable setting of the IN and OUT points.   Speed of count set by the variable jogSpeed.    The function forwardButton() shared with jogForward_btn.
    ************************************************************************************************************************************************************************************************************************************/
    if (forward_btn.isPressed()) {
      jogSpeed = 20;
      forwardButton();
    }

    /***********************************************************************************************************************************************************************************************************************************
      Similar to forward_btn.
    ***********************************************************************************************************************************************************************************************************************************/
    if (reverse_btn.isPressed()) {
      jogSpeed = 20;
      reverseButton();
    }

    /***********************************************************************************************************************************************************************************************************************************
      Enables the slow increment of the IN and OUT points.   Speed set by jogSpeed().   Shares the function forwardButton().
    ***********************************************************************************************************************************************************************************************************************************/
    if (jogForward_btn.isPressed()) {
      jogSpeed = 300;
      forwardButton();
    }

    /**********************************************************************************************************************************************************************************************************************************
      Essentially the same as jogForward - only with reverse movement.
    ***********************************************************************************************************************************************************************************************************************************/
    if (jogReverse_btn.isPressed()) {
      jogSpeed = 300;
      reverseButton();
    }
    /**********************************************************************************************************************************************************************************************************************************
      On release of the reverse_btn we must set a flag so that we can tell the last direction of travel.   In this case in reverse.   If the next travel is in reverse also then there is no backlash issue.   If however, the next
      travel is forwards we have an issue with backlash which must be calculated and accounted for.
    ***********************************************************************************************************************************************************************************************************************************/
    if (reverse_btn.justReleased() ) {
      previousDirection = 0;
    }

    /**********************************************************************************************************************************************************************************************************************************
      On release of the forward_btn we must set a flag so that we can tell the last direction of travel.   In this case forwards.   If the next travel is forwards also then there is no backlash issue.   If however, the next
      travel is reverse we have an issue with backlash which must be calculated and accounted for.
    ***********************************************************************************************************************************************************************************************************************************/
    if (forward_btn.justReleased() ) {
      previousDirection = 1;
    }

    /**********************************************************************************************************************************************************************************************************************************
      On release of the jogReverse_btn we must set a flag so that we can tell the last direction of travel.   In this case in reverse.   If the next travel is in reverse also then there is no backlash issue.   If however, the next
      travel is forwards we have an issue with backlash which must be calculated and accounted for.
    ***********************************************************************************************************************************************************************************************************************************/
    if (jogReverse_btn.justReleased() ) {
      previousDirection = 0;
    }

    /**********************************************************************************************************************************************************************************************************************************
      On release of the reverse_btn we must set a flag so that we can tell the last direction of travel.   In this case forwards.   If the next travel is forwards also then there is no backlash issue.   If however, the next
      travel is reverse we have an issue with backlash which must be calculated and accounted for.
    ***********************************************************************************************************************************************************************************************************************************/
    if (jogForward_btn.justReleased() ) {
      previousDirection = 1;
    }

    /**********************************************************************************************************************************************************************************************************************************
      All inputs for the Run have been completed and the Run can now be called i.e. the 'In' point and the 'OUT' point have both been set and we are back at the 'IN' point ready for the Run.  The 'IN' and 'OUT' points can ony be set
      if the neessary fields have been completed.  There is no other way to call PCRun().
    ***********************************************************************************************************************************************************************************************************************************/
    if (run_btn.justPressed()) {                                                                             // Initiate the full Run
      if (runSet == true) {
        PCRun();
      }
    }
    /**********************************************************************************************************************************************************************************************************************************
      Simply calls the next screen.
    ***********************************************************************************************************************************************************************************************************************************/
    if (next_btn.justPressed()) {
      screen = 3;
      draw_button_list(screen3);
    }

    /**********************************************************************************************************************************************************************************************************************************
      Returns to Screen 1 after resetting Screen 2 variables.
    ***********************************************************************************************************************************************************************************************************************************/
    if (back_btn.justPressed()) {
      in = 0;
      out = 0;
      shutters = 0;
      loops = 0;
      stepLoop = 0;
      stepsForward = 0;
      stepsBack = 0;
      stepCount = 0;
      stepsPerLoop = 0;
      stepCalc = 0;
      distance = 0;
      tempDistance = 0;
      stepCountDouble = 0.00;
      roughLoopsDouble = 0.00;
      roughLoops = 0.00;
      roughTotal = 0.00;
      remainder = 0;
      addToLoops = 0.00;
      inSet = false;
      outSet = false,
      runSet = false;
      stepControl = true;
      distanceControl = false;
      tft.fillScreen(BLUE);
      screen = 1;
      draw_button_list(screen1);
    }
  }

  /**********************************************************************************************************************************************************************************************************************************
    Utilities to go here?
  ***********************************************************************************************************************************************************************************************************************************/
  if (screen == 3) {
    update_button_list(screen3);                                                                               // Process all buttons

    if (full_btn.justPressed()) {
      digitalWrite (ms1, LOW);                                                                                 // Full steps is the default
      digitalWrite (ms2, LOW);
      digitalWrite (ms3, LOW);
      a = 2000;
      calibrationRun();
    }
    if (half_btn.justPressed()) {
      digitalWrite (ms1, HIGH);                                                                                // Half steps
      digitalWrite (ms2, LOW);
      digitalWrite (ms3, LOW);
      a = 4000;
      calibrationRun();
    }
    if (quarter_btn.justPressed()) {
      digitalWrite (ms1, LOW);                                                                                 // Quarter steps
      digitalWrite (ms2, HIGH);
      digitalWrite (ms3, LOW);
      a = 8000;
      calibrationRun();
    }
    if (eighth_btn.justPressed()) {
      digitalWrite (ms1, HIGH);                                                                                // Eighth steps
      digitalWrite (ms2, HIGH);
      digitalWrite (ms3, LOW);
      a = 16000;
      calibrationRun();
    }
    if (sixteenth_btn.justPressed()) {
      digitalWrite (ms1, HIGH);                                                                                // Sixteenth steps for BigEasyDriver
      digitalWrite (ms2, HIGH);
      digitalWrite (ms3, HIGH);
      a = 32000;
      calibrationRun();
    }
    if (runBack_btn.justPressed()) {
      calibrationRunBack();
    }
    if (main_btn.justPressed()) {
      screen = 0;
      draw_button_list(screen0);
    }
  }
}                                                                                                             // End of loop


/**********************************************************************************************************************************************************************************************************************************
  COMPLETE ACCURATE DESCRIPTION
***********************************************************************************************************************************************************************************************************************************/
bool update_button(Adafruit_GFX_Button * b, bool down)                                                       //
{
  b->press(down && b->contains(pixel_x, pixel_y));
  if (b->justReleased())
    b->drawButton(false);
  if (b->justPressed())
    b->drawButton(true);
  return down;
}

/**********************************************************************************************************************************************************************************************************************************
  COMPLETE ACCURATE DESCRIPTION
***********************************************************************************************************************************************************************************************************************************/
bool update_button_list(Adafruit_GFX_Button **pb)                                                            //
{
  bool down = Touch_getXY();
  for (int i = 0 ; pb[i] != NULL; i++) {
    update_button(pb[i], down);
  }
  return down;
}

/**********************************************************************************************************************************************************************************************************************************
  COMPLETE ACCURATE DESCRIPTION
***********************************************************************************************************************************************************************************************************************************/
void draw_button_list(Adafruit_GFX_Button **pb)                                                              //
{
  if (screen == 0) {
    drawHome();
  }
  if (screen == 1) {
    screenOne();
  }
  if (screen == 2) {
    screenTwo();
  }
  if (screen == 3) {
    screenThree();
  }
  for (int i = 0 ; pb[i] != NULL; i++) {
    pb[i]->drawButton(false);
  }
}
