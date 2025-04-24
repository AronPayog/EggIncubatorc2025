#pragma once

#include <TouchScreen.h>
#include <MCUFRIEND_kbv.h>
#include <DHT.h>
#include <virtuabotixRTC.h>


#define CLK 22
#define DAT 23
#define RST 24

#define IOSOLAR 25
#define IOHEATER 26
#define IOMOTOR 27
#define IOHUMID 28

#define IODHT 29
#define DHTTYPE DHT22


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

enum DeviceState {
  MOTOR,
  HUMID,
  HEATER,
  SOLAR,
  DEVICE_COUNT
};

enum Debug {
  DEVICES,
  DEVICES_COUNT
};

struct TimeExecution {
  String name;
  unsigned long DelayTime;
  unsigned long DurationTime;
  unsigned long startTime;
  bool active;
};

extern unsigned long MotorDelayStart;
extern unsigned long MotorActiveStart;
extern unsigned long MotorLastTime;
extern float MotorDelay;
extern float MotorActiveDuration;

extern MCUFRIEND_kbv tft;
extern TouchScreen ts;
extern TSPoint p;
extern virtuabotixRTC timertc;//CLock, IO, CeR_ST
extern DHT dht;


extern FrameState currentState, lastState;
extern SetupList currentList, lastList;
extern SetupCalibrationList currentListVar, lastListVar;
extern uint8_t TEXT_COLOR;
extern float maxtemp, lastmaxtemp, mintemp, lastmintemp;
extern int maxhumid, lastmaxhumid, minhumid, lastminhumid;
extern float temp, lasttemp;
extern int humid, lasthumid;
extern bool isSetupCalibrationFirstRun;
extern String DATE, LASTDATE, CLOCK, LASTCLOCK;


extern bool SystemRunning;
extern bool timeExecution[];
extern bool deviceState[DEVICE_COUNT];
extern bool lastdeviceState[DEVICE_COUNT];
extern bool devices[DEVICE_COUNT];

extern unsigned long timeExecutionDelay[DEVICE_COUNT];
extern unsigned long functionDelayTracker[DEVICE_COUNT];

extern uint8_t solarOnHour, solarOnMinute, solarOffHour, solarOffMinute;
// extern uint8_t temp, humid, maxtemp, mintemp, maxhumid, minhumid;
extern uint8_t currHour, currMinute, currDay, currMonth, currYear;
extern uint8_t lastHour, lastMinute, lastDay, lastMonth, lastYear;
extern uint8_t beginHour, beginMinute, beginDay, beginMonth, beginYear;
extern uint8_t toThisHour, toThisMinute, toThisDay, toThisMonth, toThisYear;

extern const uint8_t Candling[2];
extern const uint8_t UpdateHumid;
extern const uint16_t TotalOperatingDay;
