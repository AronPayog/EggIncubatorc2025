#include <virtuabotixRTC.h>
#include <DHT.h>

#define DHTPIN 5
#define DHTTYPE DHT22


virtuabotixRTC timeit(23,22,24);
DHT dht(DHTPIN, DHTTYPE);
uint8_t relays[3] = {26, 28, 29};

bool isHeaterOn;
bool isHumidOn;
bool isMotorOn;

uint8_t beginDayTime[4] = {0,0,0,0};
uint8_t lastDayTime[4] = {0,0,0,0};
uint8_t deviceUpTime[4] = {0,0,0,0};

int MotorRunDuration = 12000;
int MotorDelay = 20000;
uint32_t LastSeconds =0;
int UpTimeSeconds;

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


void updateRelayState(){
  if(digitalRead(relays[0]) == LOW){
    isHeaterOn = true;
  }else{
    isHeaterOn = false;
  }
  if(digitalRead(relays[1]) == LOW){
    isMotorOn = true;
  }else{
    isMotorOn = false;
  }
  if(digitalRead(relays[2]) == HIGH){
    isHumidOn = true;
  }else{
    isHumidOn = false;
  }

  Serial.print("( Heater ");
  if(isHeaterOn){
    Serial.print("On");
  }else{
    Serial.print("Off");
  }
  Serial.print(", Humid: ");
  if(isHumidOn){
    Serial.print("On");
  }else{
    Serial.print("Off");
  }
  Serial.print(", Motor: ");
  if(isMotorOn){
    Serial.print("On");
  }else{
    Serial.print("Off");
  }
  Serial.print(" ) ");
}
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
}
void TemperatureAndHumidityLogic(){
  if(dht.readTemperature() > 37.7){
    setRelay(0, true);
  }if(dht.readTemperature() < 35){
    setRelay(0, false);
  }
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
void MotorLogic(){

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
void Logics(){
  // testingRelays();
  MotorRotateBeta();
  // upTime();
  // printTheBeginAndEndTime();
  TemperatureAndHumidityLogic();
}
void setup(){
  Serial.begin(9600);
  for(int i=0;i<3;i++){
    pinMode(relays[i], OUTPUT);
    if(relays[i] == 29){
      digitalWrite(relays[i], LOW);
    }else{
      digitalWrite(relays[i], HIGH);
    }
  }

  timeit.updateTime();
  // timeit.setDS1302Time(0, 7, 18, 4, 13, 2, 2025);
}
void loop(){
  UpTimeSeconds = millis();
  timeit.updateTime();
  upTimeSeconds();
  upTime();
  printRtcTime();
  updateRelayState();
  Logics();
  Serial.print("\n");
  delay(1000);
}