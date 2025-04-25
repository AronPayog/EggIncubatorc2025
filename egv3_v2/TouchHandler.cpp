#include "TouchHandler.h"
#include "Config.h"
#include "UI.h"
#include "Logic.h"

void EachFrameListener(int tx, int ty){
  switch(currentState){
    case STATE_HOME:
      EggIncubatorListener(tx, ty);
      break;
    case STATE_MAIN:
      MainFrameListener(tx, ty);
      break;
    case STATE_SETUP:
      SetupListener(tx, ty);
      if(currentList == LIST1){
        SetupCalibrationListListener(tx, ty);
        SetupCalibrationAdjustVariable(tx, ty);
        delay(100);
      }
      break;
    case STATE_MENU:
      MenuListener(tx, ty);
      break;
    case STATE_TIME:
      TimeAndDateListener(tx, ty);
      break;
    case STATE_CALIBRATION:
      CalibrationListener(tx, ty);
      break;
    case STATE_ABOUT:
      AboutListener(tx, ty);
      break;
  }
}


void EggIncubatorListener(int tx, int ty){
  if(isTouchInsideCircle(240, 160, 75, tx, ty)){
    Serial.println("/> Action :: Clicked :: START BUTTON");
    SetupStartData();
    SystemRunning = true;
    MotorDelayStart = millis();
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


bool isTouchInsideCircle(int cx, int cy, int r, int tx, int ty) {
  long dx = tx - cx;
  long dy = ty - cy;
  return (dx * dx + dy * dy) <= (r * r);
}
// (x, y) Position, (w, h) Size of Button, (tx, ty) Touch Point
bool isTouchInsideRectangle(int x, int y, int w, int h, int tx, int ty) {
  return (tx >= x && tx <= (x + w) && ty >= y && ty <= (y + h));
}
