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

bool isHeaterOn;
bool isHumidOn;
bool isMotorOn;

uint8_t temp, humid, lasttemp, lasthumid;
String lastDate;
String datenow = "00/00/0000";

int lastTemp;
int lastHumid;

String lastRelayState[] = {"","",""};


void setup() {
  Serial.begin(9600);
  SetupScreen();
  MainScreen();
}

void loop() {
  temp = random(0, 120);
  humid = random(0, 120);
  destroyValues();
  drawValues(temp, humid);
  delay(1000);
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
  tft.setRotation(3);
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
  drawString(145, 20, BLACK, 3, "Egg Incubator");
  drawString(60, 60, BLACK, 3, "TEMP: ");
  drawString(60, 180, BLACK, 3, "HUMID: ");
  drawString(330, 90, BLACK, 3, "Heater");
  drawString(290, 160, BLACK, 3, "Humidifier");
  drawString(340, 230, BLACK, 3, "Motor");

  
}
void drawValues(int temp, int humid){
  drawString(60, 90, BLACK, 10, String(temp));//+char(0xF7)+"C"
  drawString(60, 210, BLACK, 10, String(humid));
  drawString(280,60, BLACK, 3, datenow);
  if(isHeaterOn){
    tft.fillRect(340, 115, 70, 30, GREEN);
  }else{
    tft.fillRect(340, 115, 70, 30, RED);
  }
  if(isHumidOn){
    tft.fillRect(340, 185, 70, 30, GREEN);
  }else{
    tft.fillRect(340, 185, 70, 30, RED);
  }
  if(isMotorOn){
    tft.fillRect(340, 255, 70, 30, GREEN);
  }else{
    tft.fillRect(340, 255, 70, 30, RED);
  }

  drawString(350, 120, BLACK, 3, "OFF");
  drawString(350, 190, BLACK, 3, "OFF");
  drawString(350, 260, BLACK, 3, "OFF");
  
  lasttemp = temp; lasthumid = humid; lastDate = datenow;
  // lastRelayState[0]="OFF";
  // lastRelayState[1]="OFF";
  // lastRelayState[2]="OFF";
}
void destroyValues(){
  drawString(60, 90, GRAY, 10, String(lasttemp));
  drawString(60, 210, GRAY, 10, String(lasthumid));
  drawString(280,60, GRAY, 3, lastDate);
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