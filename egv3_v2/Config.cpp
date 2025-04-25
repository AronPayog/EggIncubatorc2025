#include "Config.h"

MCUFRIEND_kbv tft;
TouchScreen ts = TouchScreen(xp, yp, xm, ym, 300);
TSPoint p;

virtuabotixRTC timertc(CLK, DAT, RST);// virtuabotixRTC(DATA, CLK, RST)
DHT dht(IODHT, DHTTYPE);

FrameState currentState = STATE_HOME;
FrameState lastState = STATE_HOME;

SetupList currentList = LIST1;
SetupList lastList = LIST1;

SetupCalibrationList currentListVar = NONE;
SetupCalibrationList lastListVar = NONE;

uint8_t TEXT_COLOR = BLACK;
float maxtemp, lastmaxtemp, mintemp, lastmintemp;
int maxhumid, lastmaxhumid, minhumid, lastminhumid;

float temp, lasttemp;
int humid, lasthumid;

bool isSetupCalibrationFirstRun = true;

String DATE, LASTDATE, CLOCK, LASTCLOCK;

bool SystemRunning = false;

// unsigned long DelayDevice[];
// unsigned long DurationDevice[];
bool timeExecution[] = {};


bool lastdeviceState[DEVICE_COUNT] = { false };
bool deviceState[DEVICE_COUNT] = { false }; 
bool devices[DEVICE_COUNT] = { false }; 


unsigned long timeExecutionDelay[DEVICE_COUNT] = {0};
unsigned long functionDelayTracker[DEVICE_COUNT] = {0};

uint8_t solarOnHour, solarOnMinute, solarOffHour, solarOffMinute;
// uint8_t temp, humid, maxtemp, mintemp, maxhumid, minhumid;
uint8_t currHour, currMinute, currDay, currMonth, currYear;
uint8_t lastHour, lastMinute, lastDay, lastMonth, lastYear;
uint8_t beginHour, beginMinute, beginDay, beginMonth, beginYear;
uint8_t toThisHour, toThisMinute, toThisDay, toThisMonth, toThisYear;

const uint8_t Candling[2] = {7, 14};
const uint8_t UpdateHumid = 19;
const uint16_t TotalOperatingDay = 21;

unsigned long MotorLastTime;
unsigned long MotorDelayStart = 0;
unsigned long MotorActiveStart = 0;
float MotorDelay = 5;
float MotorActiveDuration = 10;
