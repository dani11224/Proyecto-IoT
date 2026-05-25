#pragma once
#include <Arduino.h>
#include <WiFi.h>
#include <Preferences.h>
#include <DNSServer.h>
#include "config.h"

class WifiManager {
public:
  // Intenta conectarse con credenciales guardadas.
  // Si falla o no hay, inicia el portal captivo (modo AP).
  bool begin();

  bool isConfigMode() const { return _configMode; }

  void loopDNS();         // llamar en loop() solo en modo AP
  void checkResetButton();

  // Guardar credenciales (llamado desde web_server)
  static void saveCredentials(const String& ssid, const String& pass);

private:
  DNSServer  _dns;
  bool       _configMode = false;

  bool _connectSaved();
  void _startAP();
};
