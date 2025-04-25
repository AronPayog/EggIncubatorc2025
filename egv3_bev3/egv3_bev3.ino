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

bool deviceState[DEVICE_COUNT] = { false }; 
bool devices[DEVICE_COUNT] = { false }; 

unsigned long timeExecutionDelay[DEVICE_COUNT] = {0};
unsigned long functionDelayTracker[DEVICE_COUNT] = {0};

uint8_t solarOnHour, solarOnMinute, solarOffHour, solarOffMinute;
uint8_t temp, humid, maxtemp, mintemp, maxhumid, minhumid;
uint8_t currHour, currMinute, currDay, currMonth, currYear;
uint8_t lastHour, lastMinute, lastDay, lastMonth, lastYear;
uint8_t beginHour, beginMinute, beginDay, beginMonth, beginYear;
uint8_t toThisHour, toThisMinute, toThisDay, toThisMonth, toThisYear;

auto ctof = [] (float Value) -> float {
  return (Value * 9.0/5.0) + 32.0;
};



const uint8_t Candling[2] = {7, 14};
const uint8_t UpdateHumid = 19;
const uint16_t TotalOperatingDay = 21;

String DATE, LASTDATE, CLOCK, LASTCLOCK;


virtuabotixRTC timertc(CLK, DAT, RST);//CLock, IO, CeR_ST
DHT dht(IODHT, DHTTYPE);


void setup() {
  Serial.begin(9600);
  dht.begin();
  SetupDeviceIO();
}

void loop() {
  SystemTimeUpdate();
  UpdateDeviceState();
  SensorDevice();
}

void SystemTimeUpdate(){
  timertc.updateTime();
  currHour = timertc.hours;
  currMinute = timertc.minutes;
  if(currHour != lastHour || currMinute != lastMinute){
    ConvertTimeToUI();
    lastHour = currHour; lastMinute = currMinute;
  }
  if(DelayFunction(DEVICES, 3000)){
    DebugDeviceStates();
  }
}

void ConvertTimeToUI(){
  int DisplayHour = currHour;
  String suffix =" AM";
  if(currHour == 0){
    DisplayHour = 12;
  }else if(currHour == 12){
    suffix = " PM";
  }else if(currHour > 12){
    DisplayHour = currHour - 12;
    suffix = "PM";
  }
  String MinuteStr = currMinute < 10 ? "0" + String(currMinute): String(currMinute);
  CLOCK = String(DisplayHour) + ":" + MinuteStr + suffix;
}

void SetupStartData(){
  beginHour = timertc.hours;
  beginMinute = timertc.minutes;
  beginDay = timertc.dayofmonth;
  beginMonth = timertc.month;
  beginYear = timertc.year;

  PredictedEndDay(beginHour, beginMinute, beginMonth, beginDay, beginYear, TotalOperatingDay);
}

void SetupDeviceIO(){
  pinMode(IOHEATER, OUTPUT);
  pinMode(IOHUMID, OUTPUT);
  pinMode(IOSOLAR, OUTPUT);
  pinMode(IOMOTOR, OUTPUT);
}

void DebugDeviceStates() {
  Serial.println("/> Debugging..........................BEGIN");
  const char* deviceNames[DEVICE_COUNT] = {"MOTOR", "HUMID", "HEATER", "SOLAR"};
  const uint8_t ioPins[DEVICE_COUNT] = {IOMOTOR, IOHUMID, IOHEATER, IOSOLAR};

  for (int i = 0; i < DEVICE_COUNT; i++) {
    bool currentIOState = digitalRead(ioPins[i]);
    bool internalState = deviceState[i];

    Serial.print(deviceNames[i]);
    Serial.print(" | Expected: ");
    Serial.print(internalState ? "ON" : "OFF");
    Serial.print(" | Actual: ");
    Serial.print(currentIOState ? "ON" : "OFF");

    if (currentIOState != internalState) {
      Serial.println(" Mismatch!");
    } else {
      Serial.println(" Okey!");
    }
  }
  Serial.println("/> Debugging..........................END");
}

void UpdateDeviceState(){
  digitalWrite(IOHEATER, deviceState[HEATER] ? HIGH : LOW);
  digitalWrite(IOHUMID, deviceState[HUMID] ? HIGH : LOW);
  digitalWrite(IOMOTOR, deviceState[MOTOR] ? HIGH : LOW);
  digitalWrite(IOSOLAR, deviceState[SOLAR] ? HIGH : LOW);
}

void SensorDevice(){
  temp = dht.readTemperature();
  humid = dht.readHumidity();
  if(!isnan(temp) && !isnan(humid) ){
    if(ctof(temp) > ctof(maxtemp)){
      deviceState[HEATER] = false;
    }else if(ctof(temp) < ctof(mintemp)){
      deviceState[HEATER] = true;
    }
    if(humid > maxhumid){
      deviceState[HUMID] = false;
    }else if(humid < minhumid){
      deviceState[HUMID] = true;
    }
  }else{
    temp = 3; humid = 3;//3 = for Lost Connection on DHT22
  }
  if(isTimeInRange(currHour, currMinute, solarOnHour, solarOnMinute, solarOffHour, solarOffMinute)){
    deviceState[SOLAR] = true;
  } else {
    deviceState[SOLAR] = false;
  }
  if(pulseRelayEvery(IOMOTOR, 5000, 3000)){
    deviceState[MOTOR] = true;
  } else{
    deviceState[MOTOR] = false;
  }
}

bool TimeDelayExecution(uint8_t deviceIndex, uint8_t IO, unsigned long interval, unsigned long duration){
  bool currentIOState = digitalRead(IO);
  unsigned long now = millis();
  if (!currentIOState && (now - timeExecutionDelay[deviceIndex]) >= interval){
    timeExecutionDelay[deviceIndex] = now;
    return false;
  } else if (currentIOState && (now - timeExecutionDelay[deviceIndex]) >= duration){
    return true;
  }
  return currentIOState;
}

bool DelayFunction(uint8_t deviceIndex, unsigned long delayInterval) {
  unsigned long now = millis();
  if ((now - functionDelayTracker[deviceIndex]) >= delayInterval) {
    functionDelayTracker[deviceIndex] = now;
    return true;
  }
  return false;
}

bool pulseRelayEvery(uint8_t deviceIndex, unsigned long interval, unsigned long duration) {
  static unsigned long lastPulseTime[DEVICE_COUNT] = {0};
  static bool isPulsing[DEVICE_COUNT] = {false};
  static unsigned long pulseStartTime[DEVICE_COUNT] = {0};

  unsigned long now = millis();

  if (!isPulsing[deviceIndex] && now - lastPulseTime[deviceIndex] >= interval) {
    isPulsing[deviceIndex] = true;
    pulseStartTime[deviceIndex] = now;
    lastPulseTime[deviceIndex] = now;
    return true; // Turn ON
  }

  if (isPulsing[deviceIndex] && now - pulseStartTime[deviceIndex] >= duration) {
    isPulsing[deviceIndex] = false;
    return false; // Turn OFF
  }

  return isPulsing[deviceIndex]; // Maintain state
}
void PredictedEndDay(uint8_t hour, uint8_t minute, uint16_t month, uint8_t day, uint16_t year, uint8_t addDays){
  while(addDays > 0) {
    int endOfMonth = dayInMonth(month, year);
    if((day + addDays) > endOfMonth) {
      addDays -= (endOfMonth - day + 1);//1 is for adding day if begin is now not tommorrow
      day = 1;
      month++;
      if(month > 12) {
        month = 1;
        year++;
      }
    }else{
      day += addDays;
      addDays = 0;
    }
  }
  toThisHour = hour;
  toThisMinute = minute;
  toThisDay = day;
  toThisMonth = month;
  toThisYear = year;
}

bool isTimeInRange(uint8_t hour, uint8_t minute, uint8_t onHour, uint8_t onMinute, uint8_t offHour, uint8_t offMinute) {
  int now = hour * 60 + minute;
  int on = onHour * 60 + onMinute;
  int off = offHour * 60 + offMinute;

  if(on < off){
    return now >= on && now < off; //same day range
  }else {
    return now >= on || now < off; // overnigh range
  }
}
bool isLeafYear(uint16_t year){
  return ((year % 4 == 0 && year % 100 != 0) || year % 400 == 0);
}

int dayInMonth(uint8_t month, uint16_t year) {
  if(month == 2){
    return isLeafYear(year)? 29: 28;
  }else if(month == 4 || month == 6 || month == 9 || month == 11) {
    return 30;
  }else{
    return 31;
  }
}



