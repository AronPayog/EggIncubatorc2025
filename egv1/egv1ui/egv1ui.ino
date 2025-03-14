#include <Adafruit_GFX.h>
#include <MCUFRIEND_kbv.h>
#include <Fonts/FreeMonoBold9pt7b.h>
#include <TouchScreen.h> 

#define XP A3    // X+ (analog pin)
#define XM A2    // X- (analog pin)
#define YP 8     // Y+ (digital pin)
#define YM 9     // Y- (digital pin)

#define TS_MINX 100
#define TS_MAXX 900
#define TS_MINY 120
#define TS_MAXY 940
#define MINPRESSURE 10
#define MAXPRESSURE 1000

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

uint8_t temp = 0;
uint8_t humid = 0;
int lastTemp;
int lastHumid;

String lastRelayState[] = {"","",""};

TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);
Adafruit_GFX_Button myButton;

void setup() {
  Serial.begin(9600);

  uint16_t ID = tft.readID();

  if(ID == 0x6814){
    Serial.println("Found 0x6814 Screen ");
  }else{
    Serial.print("Unknown Screen ID: 0x");
    Serial.println(ID, HEX);
  }
  tft.begin(0x6814);  // Force ID6814 if it's not auto-detected
  tft.setRotation(3);  // Set screen rotation (0-3)
  HomeScreen();
  setupTouch();
}

void setupTouch(){
  pinMode(XP, OUTPUT);
  pinMode(YP, OUTPUT);
  pinMode(XM, OUTPUT);
  pinMode(YM, OUTPUT);
}

void drawInt(int16_t x, int16_t y, int msg, int16_t color, uint16_t size){
  tft.setFont(&FreeMonoBold9pt7b);
  tft.setTextSize(size);
  tft.setTextColor(color);
  tft.setCursor(x, y);
  tft.print(msg);
}


void drawString(int16_t x, int16_t y, String msg, int16_t color, uint16_t size){
  tft.setFont(&FreeMonoBold9pt7b);
  tft.setTextSize(size);
  tft.setTextColor(color);
  tft.setCursor(x, y);
  tft.print(msg);
}

void DrawTemperatureAndHumid(){
  // Erase previous values
  drawString(190, 130, String(lastTemp)+char(0xF7)+"C", WHITE, 4);
  drawString(190, 220, String(lastHumid)+"%", WHITE, 4);
  // Write new values
  drawString(190, 130, String(temp)+char(0xF7)+"C", BLACK, 4); 
  drawString(190, 220, String(humid)+"%", BLACK, 4);

  // drawString(300, 100, "C", WHITE, 5);
  // drawString(330, 90, "o", WHITE, 3);
  if(lastTemp > 100){
    tft.drawCircle(240, 160, 140, BLACK);
    tft.drawCircle(240, 160, 120, BLACK);
    tft.drawCircle(240, 160, 130, BLACK);
  }
  lastTemp = temp;
  lastHumid = humid;
}
void DrawRelayState(){
  
  // drawString(20, 30, "Heater:"+lastRelaySate[0], BLUE, 1);
  // drawString(20, 50, "Humid:"+lastRelaySate[1], BLUE, 1);
  // drawString(20, 70, "Motor:"+lastRelaySate[2], BLUE, 1);

  // drawString(20, 30, "Heater:", BLACK, 1);
  // drawString(20, 50, "Humid:", BLACK, 1);
  // drawString(20, 70, "Motor:", BLACK, 1);

  // lastRelayState[0] = 
  // lastRelayState[1] = 
  // lastRelayState[2] = 
}

void SettingScreen(){
  tft.fillScreen(BLUE);
}


void HomeScreen(){
  tft.fillScreen(BLUE);  // Set background to white
  
  tft.drawRect(10, 10, 460, 300, BLACK);

  tft.fillCircle(240, 160, 140, WHITE);
  tft.drawCircle(240, 160, 140, BLACK);
  tft.drawCircle(240, 160, 120, BLACK);
  tft.drawCircle(240, 160, 130, BLACK);

  // tft.drawLine(100, 160, 380, 160, BLACK);

  

  drawString(350, 30, "01/01/2000", BLACK, 1);
  drawString(370, 50, "12:00 AM", BLACK, 1);
  drawString(20, 290, "Menu", BLACK, 1);

  myButton.initButton(&tft, 415, 290, 110, 40, BLACK, BLUE, BLACK, "", 1);
  myButton.drawButton();
  drawString(370, 295, "Settings", BLACK, 1);

}

void touchPressure(TSPoint p) {
    if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
        int x, y;
        x = map(p.y, TS_MINY, TS_MAXY, 0, tft.width());
        y = map(p.x, TS_MAXX, TS_MINX, 0, tft.height());
        
        Serial.print("Touch detected: x=");
        Serial.print(x);
        Serial.print(" y=");
        Serial.print(y);
        Serial.print(" pressure=");
        Serial.println(p.z);

        if (myButton.contains(x, y)) {
            myButton.press(true);  // Mark it as pressed
            Serial.println("Button Pressed!");
        } else {
            myButton.press(false); // Mark it as not pressed
        }
    }
}


void loop() {
  // TSPoint p = ts.getPoint();
  // touchPressure(p);
  temp = random(0, 120);
  humid = random(0, 120);
  DrawTemperatureAndHumid();
  DrawRelayState();
  
  delay(500);
}
