#include "Logic.h"
#include "Config.h"
#include "Utils.h"

void LogicInit() {
  dht.begin();
  SetupDeviceIO();
  ConvertTimeToUI();
  Serial.print("/> LOG :: CONVERT :: TIMEUI :: ");Serial.println(CLOCK);
}

void SystemTimeUpdate(){
  timertc.updateTime();
  currHour = timertc.hours;
  currMinute = timertc.minutes;
  currDay = timertc.dayofmonth;
  currMonth = timertc.month;
  currYear = timertc.year;

  if(currHour != lastHour || currMinute != lastMinute){
    ConvertTimeToUI();
    lastHour = currHour; lastMinute = currMinute;
  }
  if(DelayFunction(DEVICES, 1000)){
    DebugDeviceStates();
    PrintDateNow();
  }

}

void PrintDateNow(){
  Serial.print("/> ");Serial.print(currHour);Serial.print(":");
  Serial.print(currMinute);Serial.print(", ");
  Serial.print(currDay);Serial.print(", ");
  Serial.print(currMonth);Serial.print(", ");
  Serial.print(currYear);Serial.print("\n");
  Serial.print("Time: ");
  Serial.print(timertc.hours);
  Serial.print(":");
  Serial.print(timertc.minutes);
  Serial.print(":");
  Serial.print(timertc.seconds);
  
  Serial.print("  Date: ");
  Serial.print(timertc.dayofmonth);
  Serial.print("/");
  Serial.print(timertc.month);
  Serial.print("/");
  Serial.println(timertc.year);
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
  if(SystemRunning){
    if(!isnan(temp) && !isnan(humid)){
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

    if(!deviceState[MOTOR] && (millis() - MotorDelayStart) >= SecondsToMillis(MotorDelay)){
      deviceState[MOTOR] = true;
      MotorActiveStart = millis();
    }
    if(deviceState[MOTOR] && (millis() - MotorActiveStart) >= SecondsToMillis(MotorActiveDuration)){
      deviceState[MOTOR] = false;
      MotorDelayStart = millis();
    } 
  }
}

bool DelayFunction(uint8_t deviceIndex, unsigned long delayInterval) {
  unsigned long now = millis();
  if ((now - functionDelayTracker[deviceIndex]) >= delayInterval) {
    functionDelayTracker[deviceIndex] = now;
    return true;
  }
  return false;
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