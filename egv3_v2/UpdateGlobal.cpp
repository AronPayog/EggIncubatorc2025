#include "UpdateGlobal.h"
#include "Config.h"

void UpdateSystemSensorData(){
  temp = dht.readTemperature();
  humid = dht.readHumidity();
}