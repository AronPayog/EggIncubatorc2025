#include <SoftwareSerial.h>

SoftwareSerial msgSerial(5, 6);

void setup() {
  Serial.begin(115200);
  msgSerial.begin(9600);
}

void loop() {
  int value = random(1, 999);
  msgSerial.println(value);
  Serial.print("Send: ");
  Serial.println(value);
  delay(1000);
}
