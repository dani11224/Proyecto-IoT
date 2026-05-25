#include "wifi_manager.h"

static const byte DNS_PORT = 53;

bool WifiManager::begin() {
  pinMode(RESET_PIN, INPUT_PULLUP);

  if (_connectSaved()) {
    _configMode = false;
    return true;
  }

  _startAP();
  return false;
}

bool WifiManager::_connectSaved() {
  Preferences prefs;
  prefs.begin("wifi", true);
  String ssid = prefs.getString("ssid", "");
  String pass = prefs.getString("pass", "");
  prefs.end();

  if (ssid.isEmpty()) {
    Serial.println("[WiFi] No hay credenciales guardadas");
    return false;
  }

  Serial.printf("[WiFi] Conectando a %s ...\n", ssid.c_str());
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid.c_str(), pass.c_str());

  int tries = 0;
  while (WiFi.status() != WL_CONNECTED && tries < 20) {
    delay(500);
    Serial.print(".");
    tries++;
  }
  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    Serial.printf("[WiFi] ✅ Conectado — IP: %s\n", WiFi.localIP().toString().c_str());
    return true;
  }

  Serial.println("[WiFi] ❌ No se pudo conectar");
  return false;
}

void WifiManager::_startAP() {
  WiFi.mode(WIFI_AP);
  WiFi.softAP(WIFI_AP_NAME);
  _dns.start(DNS_PORT, "*", WiFi.softAPIP());
  _configMode = true;
  Serial.printf("[WiFi] Modo AP — IP: %s\n", WiFi.softAPIP().toString().c_str());
}

void WifiManager::loopDNS() {
  _dns.processNextRequest();
}

void WifiManager::saveCredentials(const String& ssid, const String& pass) {
  Preferences prefs;
  prefs.begin("wifi", false);
  prefs.putString("ssid", ssid);
  prefs.putString("pass", pass);
  prefs.end();
}

void WifiManager::checkResetButton() {
  static unsigned long pressStart = 0;

  if (digitalRead(RESET_PIN) == LOW) {
    if (pressStart == 0) pressStart = millis();
    if (millis() - pressStart > 3000) {
      Serial.println("[WiFi] Borrando credenciales y reiniciando...");
      Preferences prefs;
      prefs.begin("wifi", false);
      prefs.clear();
      prefs.end();
      delay(500);
      ESP.restart();
    }
  } else {
    pressStart = 0;
  }
}
