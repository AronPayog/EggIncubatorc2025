#include "UI.h"
#include "Config.h"
#include "Utils.h"
#include "Logic.h"
#include "TouchHandler.h"

void setupData(){
  maxtemp = 37.5;
  mintemp = 35.5;
  maxhumid = 55;
  minhumid = 45;
  DATE = "00/00/0000";
}

void UIinit() {
  tft.begin(tft.readID());
  tft.setRotation(1);
  tft.fillScreen(BLACK);
  setupData();
  EggIncubatorFrame();
}

void FrameLoop() {
  p = ts.getPoint();
  pinMode(xm, OUTPUT);
  pinMode(yp, OUTPUT);

  if(currentState != lastState){
    tft.fillScreen(GRAY);
    drawCurrentScreen();
    lastState = currentState;
  }

  if (currentState == STATE_MAIN) {
    if(lasttemp != temp || lasthumid != humid){
      destroyTempHumidDisplay();
      updateTempHumidDisplay();
    }

    updateUiClock(CLOCK, LASTCLOCK, 240, 20); // drawString("99/99 AM", 240, 20, 3, WHITE, true);
    updateUiDevice();
  }


  if(currentState == STATE_SETUP){
    if(currentList != lastList){
      drawCurrentList();
    }
    if(currentList == LIST1){
      updateUIField(maxtemp, lastmaxtemp, 230, 115);
      updateUIField(mintemp, lastmintemp, 230, 168);
      updateUIField(maxhumid, lastmaxhumid, 250, 221);
      updateUIField(minhumid, lastminhumid, 250, 274);
      isSetupCalibrationFirstRun = false;
      if(currentListVar != lastListVar){
      }
    }
  }
  if(p.z > minpressure && p.z < maxpressure){
    int x = map(p.y, ts_maxy, ts_miny, 0, 490);
    int y = map(p.x, ts_minx, ts_maxx, 0, 325); 
    printTouchPoint(x, y);
    EachFrameListener(x, y);
  }
}

void drawCurrentList(){
  switch(currentList){
    case LIST1:
      CalibrationFrame();
      break;
    case LIST2:
      TimeAndDateFrame();
      break;
    case LIST3:
      tft.fillScreen(GREEN);
      break;
  }
}



void drawCurrentScreen(){
  switch(currentState){
    case STATE_HOME:
      EggIncubatorFrame();
      break;
    case STATE_MAIN:
      MainFrame();
      break;
    case STATE_SETUP:
      SetupFrame();
      CalibrationFrame();
      break;
    case STATE_MENU:
      MenuFrame();
      break;
    case STATE_TIME:
      TimeAndDateFrame();
      break;
    case STATE_CALIBRATION:
      CalibrationFrame();
      break;
    case STATE_ABOUT:
      AboutFrame();
      break;
  }
}



void SetupCalibrationAdjustVariable(int tx, int ty){
  switch(currentListVar){
    case MAXTEMP:
      SetupCalibrationAdjustListener(&maxtemp, tx, ty);
      break;
    case MINTEMP:
      SetupCalibrationAdjustListener(&mintemp, tx, ty);
      break;
    case MAXHUMID:
      SetupCalibrationAdjustListener(&maxhumid, tx, ty);
      break;
    case MINHUMID:
      SetupCalibrationAdjustListener(&minhumid, tx, ty);
      break;
  }
}



// Action Listener --------------------------------------------------------------------------------------




// UI -----------------------------------------------------------------

void EggIncubatorFrame(){
  tft.setRotation(1);
  tft.fillScreen(BLACK);
  tft.fillRect(81, 14, 318, 61, BLACK);
  drawString("Egg Incubator", 125, 33, 3, WHITE);
  tft.drawRect(87, 23, 305, 43, GRAY);
  tft.drawRect(81, 14, 318, 61, GRAY);
  drawCircleWithFrame(240, 160, 65, BLACK, "START", GREEN, 3);
  drawCircleWithFrame(379, 215, 55, BLACK, "ABOUT", RED, 2);
  drawCircleWithFrame(81, 235, 75, BLACK, "SETUP", CYAN, 4);
}
void MainFrame(){
  tft.fillScreen(GRAY);
  tft.drawRect(15, 12, 450, 295, BLACK);
  updateUiClock(CLOCK, LASTCLOCK, 240, 20);
  drawString(DATE, 340, 18, 2, WHITE);
  drawString("TEMP:", 26, 55, 3, WHITE);
  drawString("HUMID:", 26, 175, 3, WHITE);
  drawRecWithFrame(280, 73, 151, 47, RED, "MOTOR", BLACK, 3);
  drawRecWithFrame(280, 127, 151, 47, RED, "HUMID", BLACK, 3);
  drawRecWithFrame(280, 181, 151, 47, RED, "HEAT", BLACK, 3);
  drawRecWithFrame(280, 235, 151, 47, RED, "SOLAR", BLACK, 3);
  updateUiDevice();


}
void updateTempHumidDisplay(){
  drawString(String(temp)+"c", 30, 90, 7);
  drawString(String(humid)+"%", 90, 210, 7);
  lasttemp = temp; lasthumid = humid;
}
void destroyTempHumidDisplay(){
  drawString(String(lasttemp)+"c", 30, 90, 7, GRAY);
  drawString(String(lasthumid)+"%", 90, 210, 7, GRAY);
}

void SetupFrame(){
  // tft.fillScreen(GRAY);
  drawString("Setup", 185, 12, 4);
  tft.fillRect(22, 49, 127, 39, BLACK);
  tft.fillRect(163, 49, 154, 39, BLACK);
  tft.fillRect(331, 49, 127, 39, BLACK);
  drawString("Calibration", 45, 63, 1, WHITE);
  drawString("Time & Date", 198, 63, 1, WHITE);
  drawString("Other's", 378, 63, 1, WHITE);

}

void AboutFrame(){
}
void MenuFrame(){
}
void TimeAndDateFrame(){
  tft.fillScreen(BLUE);
}
void CalibrationFrame(){
  int xlistbox = 170;
  tft.drawRect(5, 92, 467, 223, BLACK);
  tft.drawRect(6, 93, 465, 221, BLACK);

  tft.fillRect(xlistbox, 101, 170, 45, WHITE);
  tft.fillRect(xlistbox, 154, 170, 45, WHITE);
  tft.fillRect(xlistbox, 207, 170, 45, WHITE);
  tft.fillRect(xlistbox, 260, 170, 45, WHITE);
  tft.drawRect(xlistbox, 101, 170, 45, BLACK);
  tft.drawRect(xlistbox, 154, 170, 45, BLACK);
  tft.drawRect(xlistbox, 207, 170, 45, BLACK);
  tft.drawRect(xlistbox, 260, 170, 45, BLACK);
  drawString("TEMP:", 30, 135, 4);
  drawString("HUMID:", 30, 240, 4);
  drawString("Max.", (xlistbox + 15), 120, 1);
  drawString("Min.", (xlistbox + 15), 173, 1);
  drawString("Max.", (xlistbox + 15), 226, 1);
  drawString("Min.", (xlistbox + 15), 279, 1);
  tft.fillCircle(420, 150, 40, WHITE);//x = 420 and y = 150, 250 (r = 40)
  tft.fillCircle(420, 250, 40, WHITE);
  tft.drawCircle(420, 150, 40, BLACK);
  tft.drawCircle(420, 250, 40, BLACK);
  tft.drawCircle(420, 150, 30, BLACK);
  tft.drawCircle(420, 250, 30, BLACK);
  drawString("+", 403, 127, 7);
  drawString("-", 403, 227, 7);
}

void updateUIField(float value, float &lastValue, int x, int y) {
  if (value != lastValue || isSetupCalibrationFirstRun) {
    destroyAndDrawValues(String(value), String(lastValue), x, y, 3, WHITE, BLACK);
    lastValue = value;
  }
}
void updateUIField(int value, int &lastValue, int x, int y) { 
  if (value != lastValue || isSetupCalibrationFirstRun) {
    destroyAndDrawValues(String(value), String(lastValue), x, y, 3, WHITE, BLACK);
    lastValue = value;
  }
}

void updateUiClock(String time, String lasttime, int x, int y){
  if(CLOCK != LASTCLOCK && currentState == STATE_MAIN){
    drawString(LASTCLOCK, 240, 20, 3, GRAY, true);
    drawString(CLOCK, 240, 20, 3, WHITE, true);
    Serial.print("/> LOG :: DRAWING :: CLOCK :: ");Serial.println(CLOCK);
    LASTCLOCK = CLOCK;
  }
}
void updateUiDevice(){
  const char* deviceNames[DEVICE_COUNT] = {"MOTOR", "HUMID", "HEAT", "SOLAR"};
  const int deviceNamesPosition[DEVICE_COUNT] = {73, 127, 181, 235};

  for(int i = 0; i < DEVICE_COUNT; i++) {
    if(deviceState[i] != lastdeviceState[i]){
      if(deviceState[i]){
        drawRecWithFrame(280, deviceNamesPosition[i], 151, 47, GREEN, String(deviceNames[i]), BLACK, 3);
        lastdeviceState[i] = deviceState[i];
      }else{
        drawRecWithFrame(280, deviceNamesPosition[i], 151, 47, RED, String(deviceNames[i]), BLACK, 3);
        lastdeviceState[i] = deviceState[i];
      }
    }
  }
}

void drawCircleWithFrame(int x, int y, int r, int color, String label, int textColor, int size){

  tft.fillCircle(x, y, r, color);
  tft.drawCircle(x, y, r, GRAY);
  tft.drawCircle(x, y, getPercent(r, 90), GRAY);
  tft.setTextSize(size); // adjust as needed
  tft.setTextColor(textColor);

  int16_t x1, y1;
  uint16_t w, h;
  tft.getTextBounds(label, 0, 0, &x1, &y1, &w, &h);

  int textX = x - w / 2;
  int textY = y - h / 2;

  tft.setCursor(textX, textY);
  tft.print(label);
}

void drawRecWithFrame(int x, int y, int w, int h, int color, String label, int textColor, int size) {
  // Outer rectangle
  tft.fillRect(x, y, w, h, color);
  tft.drawRect(x, y, w, h, BLACK);

  // Inner frame (10% inset from all sides)
  int frameMarginX = getPercent(w, 5); // 5% margin left & right
  int frameMarginY = getPercent(h, 10); // 5% margin top & bottom

  int innerX = x + frameMarginX;
  int innerY = y + frameMarginY;
  int innerW = w - 2 * frameMarginX;
  int innerH = h - 2 * frameMarginY;

  tft.drawRect(innerX, innerY, innerW, innerH, BLACK);

  // Centered text
  tft.setTextSize(size);
  tft.setTextColor(textColor);

  int16_t x1, y1;
  uint16_t textW, textH;
  tft.getTextBounds(label, 0, 0, &x1, &y1, &textW, &textH);

  int textX = x + (w - textW) / 2;
  int textY = y + (h - textH) / 2;

  tft.setCursor(textX, textY);
  tft.print(label);
}

void destroyAndDrawValues(String msg, String lastmsg, int px, int py, int size, uint16_t DestroyColor, uint16_t DrawColor){
  drawString(lastmsg, px, py, size, DestroyColor);
  drawString(msg, px, py, size, DrawColor);
}

void drawString(String msg, int x, int y, uint16_t size){
  tft.setCursor(x, y);
  // tft.setFont(&Inter_28pt_ExtraBold8pt7b);
  tft.setTextColor(TEXT_COLOR);
  tft.setTextSize(size);
  tft.print(msg);
}
void drawString(String msg, int x, int y, float size, uint16_t COLOR){
  tft.setCursor(x, y);
  // tft.setFont(&Inter_28pt_ExtraBold8pt7b);
  tft.setTextColor(COLOR);
  tft.setTextSize(size);
  tft.print(msg);
}

void drawString(String msg, int x, int y, int size, uint16_t COLOR, bool center){
  tft.setTextSize(size);
  tft.setTextColor(COLOR);

  // Get text width
  int16_t x1, y1;
  uint16_t textW, textH;
  tft.getTextBounds(msg, 0, 0, &x1, &y1, &textW, &textH);

  // Center horizontally
  int screenW = tft.width();
  int textX = (screenW - textW) / 2;

  // Use provided Y
  tft.setCursor(textX, y);
  tft.print(msg);
}
void printTouchPoint(int x, int y){
  Serial.print(" (");Serial.print(x);Serial.print(", ");Serial.print(y);
  Serial.println(")");
}