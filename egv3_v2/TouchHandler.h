#pragma once

void EachFrameListener(int tx, int ty);
void EggIncubatorListener(int tx, int ty);
void MainFrameListener(int tx, int ty);
void SetupListener(int tx, int ty);
void AboutListener(int tx, int ty);
void MenuListener(int tx, int ty);
void TimeAndDateListener(int tx, int ty);
void CalibrationListener(int tx, int ty);

void SetupCalibrationListListener(int tx, int ty);
void SetupCalibrationAdjustListener(int* var, int tx, int ty);
void SetupCalibrationAdjustListener(float* var, int tx, int ty);

bool isTouchInsideCircle(int cx, int cy, int r, int tx, int ty);
bool isTouchInsideRectangle(int x, int y, int w, int h, int tx, int ty);