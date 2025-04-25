#include <TouchScreen.h>
#include <MCUFRIEND_kbv.h>
#include "Font/Inter_28pt_ExtraBold8pt7b.h"

MCUFRIEND_kbv tft;
#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF
#define GRAY    0x8410

#define yp A1
#define xm A2
#define ym 7
#define xp 6

#define ts_minx 187
#define ts_maxx 947
#define ts_miny 179
#define ts_maxy 968

#define minpressure 10
#define maxpressure 1000

TouchScreen ts = TouchScreen(xp, yp, xm, ym, 300);
TSPoint p;


enum FrameState {
  STATE_HOME,
  STATE_MAIN,
  STATE_MENU,
  STATE_SETUP,
  STATE_ABOUT,
  STATE_TIME,
  STATE_CALIBRATION
};

enum SetupList {
  LIST1,
  LIST2,
  LIST3
};
enum SetupCalibrationList {
  MAXTEMP,
  MINTEMP,
  MAXHUMID,
  MINHUMID,
  NONE
};


FrameState currentState = STATE_MAIN;
FrameState lastState = STATE_HOME;

SetupList currentList = LIST1;
SetupList lastList = LIST1;

SetupCalibrationList currentListVar = NONE;
SetupCalibrationList lastListVar = NONE;

uint8_t TEXT_COLOR = BLACK;
float maxtemp, lastmaxtemp, mintemp, lastmintemp;
int maxhumid, lastmaxhumid, minhumid, lastminhumid;

int temp, humid, lasttemp, lasthumid;
bool isSetupCalibrationFirstRun = true;

auto getPercent = [] (float Whole, float Percentage) -> float {
  return (Percentage / 100) * Whole;
};

String DATE, LASTDATE, CLOCK, LASTCLOCK;


void setupData(){
  maxtemp = 37.5;
  mintemp = 35.5;
  maxhumid = 55;
  minhumid = 45;
  DATE = "00/00/0000";
}


void setup() {
  Serial.begin(9600);
  tft.begin(tft.readID());
  tft.setRotation(1);
  tft.fillScreen(BLACK);
  setupData();
  EggIncubatorFrame();
}

void loop() {
  temp = random(0, 100);
  humid = random(0, 120);
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

    switch(currentState){
      case STATE_HOME:
        EggIncubatorListener(x, y);
        break;
      case STATE_MAIN:
        MainFrameListener(x, y);
        break;
      case STATE_SETUP:
        SetupListener(x, y);
        if(currentList == LIST1){
          SetupCalibrationListListener(x, y);
          SetupCalibrationAdjustVariable(x, y);
        }
        break;
      case STATE_MENU:
        MenuListener(x, y);
        break;
      case STATE_TIME:
        TimeAndDateListener(x, y);
        break;
      case STATE_CALIBRATION:
        CalibrationListener(x, y);
        break;
      case STATE_ABOUT:
        AboutListener(x, y);
        break;
    }
  }
  delay(100);
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

void SetupCalibrationListListener(int tx, int ty){// 101, 154, 207, 260 (x = 170) (170, 45)
  if(isTouchInsideRectangle(170, 101, 170, 45, tx, ty)){
    destroyAndDrawValues(String(maxtemp), String(lastmaxtemp), 230, 115, 3, WHITE, GREEN);
    destroyAndDrawValues(String(mintemp), String(lastmintemp), 230, 168, 3, WHITE, BLACK);
    destroyAndDrawValues(String(maxhumid), String(lastmaxhumid), 250, 221, 3, WHITE, BLACK);
    destroyAndDrawValues(String(minhumid), String(lastminhumid), 250, 274, 3, WHITE, BLACK);

    Serial.print(" /> CalibraitonAdjustVariable :: Clicked :: MAXTEMP");
    currentListVar = MAXTEMP;
  }
  if(isTouchInsideRectangle(170, 154, 170, 45, tx, ty)){
    destroyAndDrawValues(String(mintemp), String(lastmintemp), 230, 168, 3, WHITE, GREEN);
    destroyAndDrawValues(String(maxtemp), String(lastmaxtemp), 230, 115, 3, WHITE, BLACK);
    destroyAndDrawValues(String(maxhumid), String(lastmaxhumid), 250, 221, 3, WHITE, BLACK);
    destroyAndDrawValues(String(minhumid), String(lastminhumid), 250, 274, 3, WHITE, BLACK);
    Serial.print(" /> CalibraitonAdjustVariable :: Clicked :: MINTEMP");
    currentListVar = MINTEMP;
  }
  if(isTouchInsideRectangle(170, 207, 170, 45, tx, ty)){
    destroyAndDrawValues(String(maxhumid), String(lastmaxhumid), 250, 221, 3, WHITE, GREEN);
    destroyAndDrawValues(String(maxtemp), String(lastmaxtemp), 230, 115, 3, WHITE, BLACK);
    destroyAndDrawValues(String(mintemp), String(lastmintemp), 230, 168, 3, WHITE, BLACK);
    destroyAndDrawValues(String(minhumid), String(lastminhumid), 250, 274, 3, WHITE, BLACK);
    Serial.print(" /> CalibraitonAdjustVariable :: Clicked :: MAXHUMID");
    currentListVar = MAXHUMID;
  }
  if(isTouchInsideRectangle(170, 260, 170, 45, tx, ty)){
    destroyAndDrawValues(String(minhumid), String(lastminhumid), 250, 274, 3, WHITE, GREEN);
    destroyAndDrawValues(String(maxtemp), String(lastmaxtemp), 230, 115, 3, WHITE, BLACK);
    destroyAndDrawValues(String(mintemp), String(lastmintemp), 230, 168, 3, WHITE, BLACK);
    destroyAndDrawValues(String(maxhumid), String(lastmaxhumid), 250, 221, 3, WHITE, BLACK);
    Serial.print(" /> CalibraitonAdjustVariable :: Clicked :: MINHUMID");
    currentListVar = MINHUMID;
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
void SetupCalibrationAdjustListener(int* var, int tx, int ty){//x = 420 and y = 150, 250 (r = 40)
  if(isTouchInsideCircle(420, 150, 40, tx, ty)){
    *var+=1;
    Serial.print("/> CalibrationAdjustListener :: Clicked :: ADD :: ");Serial.println(*var);
  }
  if(isTouchInsideCircle(420, 250, 40, tx, ty)){
    *var-=1;
    Serial.print("/> CalibrationAdjustListener :: Clicked :: MINUS :: ");Serial.println(*var);
  }
}
void SetupCalibrationAdjustListener(float* var, int tx, int ty){//x = 420 and y = 150, 250 (r = 40)
  if(isTouchInsideCircle(420, 150, 40, tx, ty)){
    *var+=.1;
    Serial.print("/> CalibrationAdjustListener :: Clicked :: ADD :: ");Serial.println(*var);
  }
  if(isTouchInsideCircle(420, 250, 40, tx, ty)){
    *var-=.1;
    Serial.print("/> CalibrationAdjustListener :: Clicked :: MINUS :: ");Serial.println(*var);
  }
}


// Action Listener --------------------------------------------------------------------------------------

void EggIncubatorListener(int tx, int ty){
  if(isTouchInsideCircle(240, 160, 75, tx, ty)){
    Serial.println("/> Action :: Clicked :: START BUTTON");
    currentState = STATE_MAIN;
  }
  if(isTouchInsideCircle(379, 215, 55, tx, ty)){
    Serial.println("/> Action :: Clicked :: ABOUT");
    currentState = STATE_ABOUT;
  }
  if(isTouchInsideCircle(81, 235, 75, tx, ty)){
    Serial.println("/> Action :: Clicked :: SETUP");
    currentState = STATE_SETUP;
  }
}

void MainFrameListener(int tx, int ty){
  if(isTouchInsideRectangle(12, 12, 40, 97, tx, ty)){
    currentState = STATE_SETUP;
  }
}
void SetupListener(int tx, int ty){
  if(isTouchInsideRectangle(428, 0, 52, 56, tx, ty)){
    Serial.println("/> Action :: Clicked :: Setup Exit Button");
    currentState = STATE_HOME;
    isSetupCalibrationFirstRun = true;
  }
}
void AboutListener(int tx, int ty){
  if(isTouchInsideRectangle(428, 0, 52, 56, tx, ty)){
    Serial.println("/> Action :: Clicked :: About Exit Button");
    currentState = STATE_HOME;
  }
}
void MenuListener(int tx, int ty){
  if(isTouchInsideRectangle(428, 0, 52, 56, tx, ty)){
    Serial.println("/> Action :: Clicked :: Menu Exit Button");
    currentState = STATE_HOME;
  }
}
void TimeAndDateListener(int tx, int ty){
  if(isTouchInsideRectangle(428, 0, 52, 56, tx, ty)){
    Serial.println("/> Action :: Clicked :: TimeDate Exit Button");
    currentState = STATE_HOME;
  }
}
void CalibrationListener(int tx, int ty){
  if(isTouchInsideRectangle(428, 0, 52, 56, tx, ty)){
    Serial.println("/> Action :: Clicked :: Calibration Exit Button");
    currentState = STATE_HOME;
  }
}

bool isTouchInsideCircle(int cx, int cy, int r, int tx, int ty) {
  long dx = tx - cx;
  long dy = ty - cy;
  return (dx * dx + dy * dy) <= (r * r);
}
// (x, y) Position, (w, h) Size of Button, (tx, ty) Touch Point
bool isTouchInsideRectangle(int x, int y, int w, int h, int tx, int ty) {
  return (tx >= x && tx <= (x + w) && ty >= y && ty <= (y + h));
}

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
  drawString("99/99 AM", 240, 20, 3, WHITE, true);
  drawString(DATE, 340, 18, 2, WHITE);
  drawString("TEMP:", 26, 55, 3, WHITE);
  drawString("HUMID:", 26, 175, 3, WHITE);
  drawRecWithFrame(280, 73, 151, 47, WHITE, "MOTOR", BLACK, 3);
  drawRecWithFrame(280, 127, 151, 47, WHITE, "HUMID", BLACK, 3);
  drawRecWithFrame(280, 181, 151, 47, WHITE, "HEAT", BLACK, 3);
  drawRecWithFrame(280, 235, 151, 47, WHITE, "SOLAR", BLACK, 3);

}
void updateTempHumidDisplay(){
  drawString(String(temp)+"c", 30, 90, 9);
  drawString(String(humid)+"%", 30, 210, 9);
  lasttemp = temp; lasthumid = humid;
}
void destroyTempHumidDisplay(){
  drawString(String(lasttemp)+"c", 30, 90, 9, GRAY);
  drawString(String(lasthumid)+"%", 30, 210, 9, GRAY);
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
  if(CLOCK != lASTCLOCK){
    drawString(LASTCLOCK, 240, 20, 3, GRAY, true);
    drawString(CLOCK, 240, 20, 3, WHITE, true);
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
