#include <virtuabotixRTC.h>
#include <DHT.h>
#include <Adafruit_GFX.h>
#include <MCUFRIEND_kbv.h>
#include <Fonts/FreeMonoBold9pt7b.h>

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

#define DHTPIN 31
#define DHTTYPE DHT22
#define HUMIDPIN 30
#define HumidPin 30


virtuabotixRTC timeit(23,22,24);

DHT dht(DHTPIN, DHTTYPE);
// DHT dht(0, DHT22);
uint8_t relays[3] = {26, 28, 29};

bool isHeaterOn;  
bool isHumidOn;
bool isMotorOn;
float temp, humid, lasttemp, lasthumid;
String lastDate;
String datenow = "00/00/0000";
int lastTemp;
int lastHumid;
String lastRelayState[] = {"","",""};
int lastHours, lastMinutes;
String lastFrameSymble;

bool humidifierState = false;

uint8_t beginDayTime[4] = {0,0,0,0};
uint8_t lastDayTime[4] = {0,0,0,0};
uint8_t deviceUpTime[4] = {0,0,0,0};

int MotorRunDuration = 12000;
int MotorDelay = 20000;
uint32_t LastSeconds = 0;
int UpTimeSeconds;

int currHour;
int currMinute;
int toThisDay;
int toThisMonth;
int toThisYear;

int operatingdays = 840;

int candling[2] = {7, 14};
int calibrationHumidAndTemp = 19;

uint8_t maximumTemp = 37.5;
uint8_t minimumTemp = 35.5;
uint8_t maximumHumid = 55;
uint8_t minimumHumid = 45;

const int controlPin = 33;

int onHour = 19;     // 7 PM
int onMinute = 45;
int offHour = 5;     // 5 AM
int offMinute = 30;


void setRelay(uint8_t relay, bool relayState){
  digitalWrite(relays[relay], relayState);
}
void testRelays(){
  Serial.println("Relay Testing Setup");
  for(int i=0;i<*(&relays + 1)-relays;i++){
    digitalWrite(relays[i], 0x1);
    delay(1000);
    digitalWrite(relays[i], 0x0);
    delay(1000);
    Serial.println(relays[i]);
  }
  Serial.println("Relays Setup Done Testing@");
  delay(5000);
}

void SetupScreen(){
  uint16_t ID = tft.readID();

  if(ID == 0x6814){
    Serial.println("Found 0x6814 Screen ");
  }else{
    Serial.print("Unknown Screen ID: 0x");
    Serial.println(ID, HEX);
  }
  tft.begin(0x6814);
  tft.setRotation(1);
  tft.fillScreen(BLACK);
}

void MainScreen(){
  tft.fillScreen(GRAY);
  tft.drawRect(10, 10, 460, 300, BLACK);
  tft.drawRect(53, 12, 415, 296, BLACK);
  tft.drawRect(53, 12, 415, 296, BLACK);
  tft.drawLine(53, 50, 465, 50, BLACK);
  tft.fillRect(12, 12, 40, 97, BLACK);
  tft.fillRect(12, 112, 40, 96, BLACK);
  tft.fillRect(12, 211, 40, 97, BLACK);
  drawString(120, 20, BLACK, 3, "Egg Incubator");
  drawString(60, 60, BLACK, 3, "TEMP: ");
  drawString(60, 180, BLACK, 3, "HUMID: ");
  drawString(330, 90, BLACK, 3, "Heater");
  drawString(290, 160, BLACK, 3, "Humidifier");
  drawString(340, 230, BLACK, 3, "Motor");

  
}
void drawValues(float temp, float humid){
  
  drawString(60, 90, BLACK, 8, String(temp));//+char(0xF7)+"C"
  drawString(60, 210, BLACK, 8, String(humid));
  drawString(280,60, BLACK, 3, datenow);
  drawString(370, 20, BLACK, 3, String(timeit.hours)+":"+String(timeit.minutes));
  if((timeit.seconds % 2) == 0){
    drawString(95, 20, BLACK, 3, "*");
    lastFrameSymble = "*";
  }else{
    drawString(95, 20, BLACK, 3, "#");
    lastFrameSymble = "#";
  }
  if(isHeaterOn){
    tft.fillRect(340, 115, 70, 30, GREEN);
    drawString(350, 120, BLACK, 3, " ON");
  }else{
    tft.fillRect(340, 115, 70, 30, RED);
    drawString(350, 120, BLACK, 3, "OFF");
  }
  if(humidifierState){
    tft.fillRect(340, 185, 70, 30, GREEN);
    drawString(350, 190, BLACK, 3, " ON");
  }else{
    tft.fillRect(340, 185, 70, 30, RED);
    drawString(350, 190, BLACK, 3, "OFF");
  }
  if(isMotorOn){
    tft.fillRect(340, 255, 70, 30, GREEN);
    drawString(350, 260, BLACK, 3, " ON");
  }else{
    tft.fillRect(340, 255, 70, 30, RED);
    drawString(350, 260, BLACK, 3, "OFF");
  }

  lasttemp = temp; lasthumid = humid; lastDate = datenow;
  lastHours = timeit.hours; lastMinutes = timeit.minutes;
}
void destroyValues(){
  drawString(60, 90, GRAY, 8, String(lasttemp));
  drawString(60, 210, GRAY, 8, String(lasthumid));
  drawString(280,60, GRAY, 3, lastDate);
  drawString(370, 20, GRAY, 3, String(lastHours)+":"+String(lastMinutes));
  drawString(95, 20, GRAY, 3, lastFrameSymble);
  // drawString(350, 120, GRAY, 3, lastRelayState[0]);
  // drawString(350, 190, GRAY, 3, lastRelayState[1]);
  // drawString(350, 260, GRAY, 3, lastRelayState[2]);

}
void drawString(uint16_t x,uint16_t y,uint16_t color, uint16_t textsize, String msg){
  tft.setCursor(x, y);
  tft.setTextColor(color);
  tft.setTextSize(textsize);
  tft.print(msg);
}


// void updateRelayState(){
//   if(digitalRead(relays[0]) == LOW){
//     isHeaterOn = true;
//   }else{
//     isHeaterOn = false;
//   }
//   if(digitalRead(relays[1]) == LOW){
//     isMotorOn = true;
//   }else{
//     isMotorOn = false;
//   }
//   if(digitalRead(relays[2]) == HIGH){
//     isHumidOn = true;
//   }else{
//     isHumidOn = false;
//   }

//   Serial.print("( Heater ");
//   if(isHeaterOn){
//     Serial.print("On");
//   }else{
//     Serial.print("Off");
//   }
//   Serial.print(", Humid: ");
//   if(humidifierState){
//     Serial.print("On");
//   }else{
//     Serial.print("Off");
//   }
//   Serial.print(", Motor: ");
//   if(isMotorOn){
//     Serial.print("On");
//   }else{
//     Serial.print("Off");
//   }
//   Serial.print(" ) ");
// }
void testingRelays(){
  if(timeit.minutes & 3 == 0){
    setRelay(2, true);
    setRelay(1, false);
    setRelay(0, false);
  }
  else if(timeit.minutes % 2 == 0){
    setRelay(2, false);
    setRelay(1, false);
    setRelay(0, true);
  }
  else if(timeit.minutes % 2 == 1){
    setRelay(2, false);
    setRelay(1, true);
    setRelay(0, false);
  }
}

void setDayTimeEnd(){
  beginDayTime[0] = timeit.month;
  beginDayTime[1] = timeit.dayofmonth;
  beginDayTime[2] = timeit.hours;
  beginDayTime[3] = timeit.minutes;

  lastDayTime[0] = beginDayTime[0];
  lastDayTime[1] = beginDayTime[1] + 21;
  lastDayTime[2] = beginDayTime[2];
  lastDayTime[3] = beginDayTime[3];
  Serial.println("Date Begin and EndTIme Has Been Set!!");
  printTheBeginAndEndTime();
}
void printTheBeginAndEndTime(){
  Serial.print("( ");
  for(uint8_t i = 0;i<*(&beginDayTime + 1)-beginDayTime;i++){
    Serial.print(beginDayTime[i]);
    Serial.print(":");
  }
  Serial.print(" -> ");
  for(uint8_t i = 0;i<*(&lastDayTime + 1)-lastDayTime;i++){
    Serial.print(lastDayTime[i]);
    Serial.print(":");
  }
  Serial.print(" ) ");
}

void upTime(){
  timeit.updateTime();
  deviceUpTime[0] = timeit.month - beginDayTime[0];
  deviceUpTime[1] = timeit.dayofmonth - beginDayTime[1];
  deviceUpTime[2] = timeit.hours - beginDayTime[2];
  deviceUpTime[3] = timeit.minutes - beginDayTime[3];

  Serial.print(" ( Uptime: ");Serial.print(deviceUpTime[0]);
  Serial.print(":");Serial.print(deviceUpTime[1]);
  Serial.print(":");Serial.print(deviceUpTime[2]);
  Serial.print(":");Serial.print(deviceUpTime[3]);
  Serial.print(") ");
}
void upTimeSeconds(){
  Serial.print(" (");
  Serial.print(UpTimeSeconds);
  Serial.print(") ");
  char dateString[11];
  sprintf(dateString, "%02d/%02d/%04d", timeit.dayofmonth, timeit.month, timeit.year);
  datenow = String(dateString);
}

void ChangeHumidifierState(){
  int changeStateDelay = 25;
  digitalWrite(HUMIDPIN, HIGH);
  delay(changeStateDelay);
  digitalWrite(HUMIDPIN, LOW);
  delay(changeStateDelay);
  digitalWrite(HUMIDPIN, HIGH);
  delay(changeStateDelay);
  digitalWrite(HUMIDPIN, LOW);
  delay(changeStateDelay);
}

void TemperatureAndHumidityLogic(){
  if(dht.readTemperature() > 37.7){
    setRelay(0, true);
  }if(dht.readTemperature() < 35){
    setRelay(0, false);
  }
  //55 - 60 operating 1 to 18 days
  //80 - 95 operating 19 to 21 days
  if(dht.readHumidity() > 60 && humidifierState){
    humidSwitchOff();
    Serial.print("( Switching Off Humid!! )");
  }else if(dht.readHumidity() < 55 && !humidifierState){
    humidSwitchOn();
    Serial.print("( Switching On Humid!! )");
  }
}

void humidSwitchOff(){
  uint8_t processdelay = 40;
  digitalWrite(HumidPin, HIGH);
  delay(processdelay);
  digitalWrite(HumidPin, LOW);
  delay(processdelay);
  digitalWrite(HumidPin, HIGH);
  delay(processdelay);
  digitalWrite(HumidPin, LOW);
  delay(processdelay);
  humidifierState = false;
}
void humidSwitchOn(){
  uint8_t processdelay = 30;
  digitalWrite(HumidPin, HIGH);
  delay(processdelay);
  digitalWrite(HumidPin, LOW);
  delay(processdelay);
  humidifierState = true;
}

void MotorRotateBeta(){
  if(!isMotorOn && (millis() - LastSeconds) >= MotorDelay){
    setRelay(1, false);
    LastSeconds = millis();
  }
  if(isMotorOn && (millis() - LastSeconds) >= MotorRunDuration){
    setRelay(1, true);
  }
}

void printRtcTime(){
  Serial.print("Current Time: ");
  Serial.print(timeit.hours);
  Serial.print(":");
  Serial.print(timeit.minutes);
  Serial.print(":");
  Serial.print(timeit.seconds);
  Serial.print(" ");

  // Display the current date
  Serial.print(timeit.dayofmonth);
  Serial.print("/");
  Serial.print(timeit.month);
  Serial.print("/");
  Serial.print(timeit.year);
  
}
void readTemperatureAndHumid(){
  temp = dht.readTemperature();
  humid = dht.readHumidity();
  if(isnan(temp) || isnan(humid)){
    Serial.print("( ERROR: DHT22 Lost Connection!! )");
  }else{  
    Serial.print("( Temp: ");Serial.print(temp);
    Serial.print(" , Humid: ");Serial.print(humid);
    Serial.print(" )");
  }
}
void switchSolarMode(){
  if (isTimeInRange(currHour, currMinute, onHour, onMinute, offHour, offMinute)) {
    digitalWrite(controlPin, HIGH);  // ON
  } else {
    digitalWrite(controlPin, LOW);   // OFF
  }
}

bool isTimeInRange(int h, int m, int onH, int onM, int offH, int offM) {
  int now = h * 60 + m;
  int on = onH * 60 + onM;
  int off = offH * 60 + offM;

  if (on < off) {
    return now >= on && now < off; // same day range
  } else {
    return now >= on || now < off; // overnight range
  }
}
void Logics(){
  // testingRelays();
  MotorRotateBeta();
  // upTime();
  // printTheBeginAndEndTime();
  // readTemperatureAndHumid();
  TemperatureAndHumidityLogic();

  if(timeit.dayofmonth == calibrationHumidAndTemp){
    maximumTemp = 37.5;
    minimumTemp = 35.5;
    maximumHumid = 75;
    minimumHumid = 65;
  }
}
void setup(){
  Serial.begin(9600);
  dht.begin();
  for(int i=0;i<3;i++){
    pinMode(relays[i], OUTPUT);
    if(relays[i] == 29){
      digitalWrite(relays[i], LOW);
    }else{
      digitalWrite(relays[i], HIGH);
    }
  }
  pinMode(HUMIDPIN, OUTPUT);
  pinMode(HumidPin, OUTPUT);
  SetupScreen();
  MainScreen();
  drawValues(temp, humid);
  timeit.updateTime();
  // timeit.setDS1302Time(0, 26, 23, 3, 12, 3, 2025);
  // tft.fillScreen("BLUE");
  delay(10000);
}
void loop(){
  UpTimeSeconds = millis();
  timeit.updateTime();
  currHour = timeit.hours;
  currMinute = timeit.minutes;
  // upTimeSeconds();
  // upTime();
  // printRtcTime();
  // updateRelayState();
  Logics();
  Serial.print("\n");

  // destroyValues();
  // drawValues(temp, humid);
  // ChangeHumidifierState();

  delay(1000);
}