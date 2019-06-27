/**********************************************************************************************************************************************************************************************************************************
  The field descriptors and layouts of any field buttons or other spaces for the variable contents.   setCursor format (x-axis, y-axis)  drawRoundRect(x-axis, y-axis, Width, Height, Corner Radius, Colour).
***********************************************************************************************************************************************************************************************************************************/
void screenOne() {
  tft.drawRect(0, 0, 480, 320, WHITE);                                                                        //Page border
  tft.fillRect(2, 3, 471, 315, BLUE);
  tft.setCursor(165, 10);
  tft.setTextColor(WHITE);
  tft.setTextSize(2);
  tft.print("INPUT MENU");
  tft.setCursor(10, 50);
  tft.print("Lens Type");
  tft.setCursor(10, 98);
  tft.print("Sensor");
  tft.setCursor(10, 146);
  tft.print("FStop");
  tft.setCursor(10, 194);
  tft.print("Frame Size");
  tft.setCursor(10, 242);
  tft.print("Leadscrew");
  tft.setCursor(240, 50);
  tft.print("Stepper");
  tft.setCursor(240, 98);
  tft.print("Microsteps");
  tft.setCursor(240, 146);
  tft.print("Run Type");
  tft.setCursor(240, 194);
  tft.print("Step Delay");
  tft.setCursor(240, 242);
  tft.print("Flash Delay");
  tft.setCursor(10, 50);
  tft.drawRoundRect(139, 188, 75, 30, 4, CYAN);                                                              // For Framesize
  tft.fillRoundRect(139, 188, 75, 30, 4, CYAN);
  tft.setCursor(150, 195);
  tft.setTextColor(BLACK);
  tft.print(y / 10);                                                                                         // Framesize in 1/10ths of mm
}

void screenTwo() {
  tft.drawRect(0, 0, 480, 320, WHITE);                                                                       //Page border
  tft.fillRect(2, 3, 471, 315, BLUE);
  tft.setCursor(175, 10);
  tft.setTextColor(WHITE);
  tft.setTextSize(2);
  tft.print("RUN CONTROLS");
  tft.setCursor(15, 60);
  tft.print("Run Steps");
  tft.setCursor(15, 100);
  tft.print("Shutters");
  tft.setCursor(15, 140);
  tft.print("EfStop");
  tft.setCursor(15, 180);
  tft.print("Magnify");
  tft.setCursor(260, 60);
  tft.print("IN Point");
  tft.setCursor(260, 100);
  tft.print("OUT Point");
  tft.setCursor(260, 140);
  tft.print("D.O.F.");
  tft.setCursor(260, 180);
  tft.print("Distance");
  tft.fillRoundRect(130, 52, 90, 30, 4, WHITE);                                                             // Steps
  tft.fillRoundRect(130, 92, 90, 30, 4, WHITE);                                                             // Shutters
  tft.fillRoundRect(130, 132, 90, 30, 4, CYAN);                                                             // eFStop
  tft.fillRoundRect(130, 172, 90, 30, 4, CYAN);                                                             // Magnification
  tft.fillRoundRect(380, 52, 90, 30, 4, WHITE);                                                             // IN
  tft.fillRoundRect(380, 92, 90, 30, 4, WHITE);                                                             // OUT
  tft.fillRoundRect(380, 132, 90, 30, 4, CYAN);                                                             // DOF
  tft.fillRoundRect(380, 172, 90, 30, 4, WHITE);                                                            // Distance
  tft.setTextColor(BLACK);
  tft.drawRoundRect(165, 215, 150, 50, 4, WHITE);                                                           // Message Box
  tft.fillRoundRect(168, 218, 144, 44, 4, CYAN);
}

void screenThree() {
  tft.drawRect(0, 0, 480, 320, WHITE);                                                                      //Page border
  tft.fillRect(2, 3, 471, 315, BLUE);
  tft.setCursor(175, 10);
  tft.setTextColor(WHITE);
  tft.setTextSize(2);
  tft.print("CALIBRATION");
}
