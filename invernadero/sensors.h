#pragma once
#include <Arduino.h>
#include <DHT.h>
#include "config.h"

struct SensorData {
  float temperatura;
  float humedad;
  int   luz;          // 0-4095
  bool  dhtOk;        // false si lectura inválida
};

class Sensors {
public:
  void       begin();
  SensorData read();

private:
  DHT _dht{PIN_DHT, DHT22};
};
