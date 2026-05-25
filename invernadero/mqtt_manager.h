#pragma once
#include <Arduino.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <time.h>
#include "config.h"
#include "sensors.h"
#include "actuators.h"

// Umbrales ajustables desde el dashboard
struct Umbrales {
  float tempMax    = UMBRAL_TEMP_MAX;
  float humedadMin = UMBRAL_HUMEDAD_MIN;
  int   luzMin     = UMBRAL_LUZ_MIN;
};

class MqttManager {
public:
  void begin(Actuators* actuators);
  void loop();

  void publishSensors(const SensorData& d);
  void publishStatus(const SensorData& d);

  bool      modoManual() const { return _manual; }
  void      setModo(const String& modo) { _manual = (modo == "manual"); }
  Umbrales& umbrales()         { return _umbrales; }

private:
  WiFiClientSecure _wifiClient;
  PubSubClient     _client{_wifiClient};
  Actuators*       _actuators = nullptr;
  bool             _manual    = false;
  Umbrales         _umbrales;

  void _reconnect();
  void _syncNTP();

  static void _callback(char* topic, byte* payload, unsigned int length);
  static MqttManager* _instance;   // para el callback estático
  void _handleMessage(const String& topic, const String& payload);
};
