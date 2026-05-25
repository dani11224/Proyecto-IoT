#include "sensors.h"

void Sensors::begin() {
  _dht.begin();
  // ADC del ESP32: pin 34 es solo entrada, no necesita pinMode especial
  analogReadResolution(12);   // 0-4095
}

SensorData Sensors::read() {
  SensorData d;

  d.temperatura = _dht.readTemperature();
  d.humedad     = _dht.readHumidity();
  d.luz         = analogRead(PIN_LDR);
  d.dhtOk       = !(isnan(d.temperatura) || isnan(d.humedad));

  if (!d.dhtOk) {
    d.temperatura = -999;
    d.humedad     = -999;
    Serial.println("[Sensors] ⚠️  Lectura DHT22 inválida");
  }

  return d;
}
