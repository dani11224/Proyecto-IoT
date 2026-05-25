#pragma once
#include <Arduino.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include "config.h"
#include "sensors.h"
#include "actuators.h"
#include "mqtt_manager.h"
#include "wifi_manager.h"

class WebServerManager {
public:
  void begin(Sensors* sensors, Actuators* actuators, MqttManager* mqtt, bool configMode);
  void loop();

private:
  WebServer  _server{80};
  Sensors*   _sensors   = nullptr;
  Actuators* _actuators = nullptr;
  MqttManager* _mqtt    = nullptr;
  bool       _configMode = false;

  // ── Handlers modo CONFIG (portal captivo) ──
  void _handleSetupRoot();
  void _handleScan();
  void _handleConnect();
  void _handleReset();

  // ── Handlers modo OPERACIÓN (dashboard) ──
  void _handleDashboard();
  void _handleApiStatus();    // GET  /api/status  — healthcheck
  void _handleApiSensors();   // GET  /api/sensors
  void _handleApiActuators(); // GET  /api/actuators
  void _handleApiControl();   // POST /api/control  — modo + actuadores
  void _handleApiUmbrales();  // POST /api/umbrales

  // ── Helpers ──
  String _buildDashboardHTML();
  String _buildSetupHTML();
  void   _cors();
};
