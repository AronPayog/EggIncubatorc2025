#include "UI.h"
#include "Logic.h"
#include "UpdateGlobal.h"

void setup() {
  Serial.begin(9600);
  UIinit();
  LogicInit();
  // timertc.setDS1302Time(0, 8, 1, 5, 25, 4, 2025);
}

void loop() {
  timertc.updateTime();

  UpdateSystemSensorData();
  SystemTimeUpdate();
  SensorDevice();
  UpdateDeviceState();
  FrameLoop();
}
