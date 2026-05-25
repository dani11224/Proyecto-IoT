#include "mqtt_manager.h"
#include <WiFi.h>

MqttManager* MqttManager::_instance = nullptr;

// ─── Certificado raíz Let's Encrypt (HiveMQ Cloud lo usa)
// Válido hasta 2035 — actualizar si expira
static const char ROOT_CA[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIFazCCA1OgAwIBAgIRAIIQz7DSQONZRGPgu2OCiwAwDQYJKoZIhvcNAQELBQAw
TzELMAkGA1UEBhMCVVMxKTAnBgNVBAoTIEludGVybmV0IFNlY3VyaXR5IFJlc2Vh
cmNoIEdyb3VwMRUwEwYDVQQDEwxJU1JHIFJvb3QgWDEwHhcNMTUwNjA0MTEwNDM4
WhcNMzUwNjA0MTEwNDM4WjBPMQswCQYDVQQGEwJVUzEpMCcGA1UEChMgSW50ZXJu
ZXQgU2VjdXJpdHkgUmVzZWFyY2ggR3JvdXAxFTATBgNVBAMTDElTUkcgUm9vdCBY
MTCCAiIwDQYJKoZIhvcNAQEBBQADggIPADCCAgoBggIBAK3oJHP0FDfzm54rVygc
h77ct984kIxuPOZXoHj3dcKi/vVqbvYATyjb3miGbESTtrFj/RQSa78f0uoxmyF+
0TM8ukj13Xnfs7j/EvEhmkvBioZxaUpmZmyPfjxwv60pIgbz5MDmgK7iS4+3mX6U
A5/TR5d8mUgjU+g4rk8Kb4Mu0UlXjIB0ttov0DiNewNwIRt18jA8+o+u3dpjq+sW
T8KOEUt+zwvo/7V3LvSye0rgTBIlDHCNAymg4VMk7BPZ7hm/ELNKjD+Jo2FR3qyH
B5T0Y3HsLuJvW5iB4YlcNHlsdu87kGJ55tukmi8mxdAQ4Q7e2RCOFvu396j3x+UC
B5iPNgiV5+I3lg02dZ77DnKxHZu8A/lJBdiB3QW0KtZB6awBdpUKD9jf1b0SHzUv
KBds0pjBqAlkd25HN7rOrFleaJ1/ctaJxQZBKT5ZPt0m9STJEadao0xAH0ahmbWn
OlFuhjuefXKnEgV4We0+UXgVCwOPjdAvBbI+e0ocS3MFEvzG6uBQE3xDk3SzynTn
jh8BCNAw1FtxNrQHusEwMFxIt4I7mKZ9YIqioymCzLq9gwQbooMDQaHWBfEbwrbw
qHyGO0aoSCqI3Haadr8faqU9GY/rOPNk3sgrDQoo//fb4hVCEEWyqYh0VlRvAQID
AQABo0IwQDAPBgNVHRMBAf8EBTADAQH/MA4GA1UdDwEB/wQEAwIBBjAdBgNVHQ4E
FgQUebRZ5nu25eQBc4AIiMgaWPbpm24wDQYJKoZIhvcNAQELBQADggIBAFrJRuOH
7tDxMNp5RpSzaANBGIhwgxV8iqIFHBmEiDE3EHUY0LOJXB0cYPnCbgx88Gk6cC9
g5ICzBUHMh8FHT3OHDe7EEQE0FDLpTRMQiV0cOFMOkUXS3IXv62T4mJMDH3HMUJ
/YXP9YfMxMfhvE5RcaOm/8VKQdCF8P2GH6oKDqPv7MAIGa5GHzgWBYkNgYMxXVD
4vT5XYQF6bx5zX5y9L2JR2v8GZJL0ELe3m0pEJrVkFEzKB7nFk5bI6MZjpN7YCk
PfNY/bXOH6QYhKBCjST0hxG56FHBZ1VVyCwGPYmibqUL2sCOsFHEoZnMmZKxrqx
f8XiQ4lQQUMv4HlQEtIBFkIxHl5MBCqVkRBl93IILl5NeJ2JlBRiUgsTxKgX8oZ
bOZ+UPmCxuFGOCMTlFRoxr8AEUBf1H7JlNMARMhZR5I6YFJOTp3FtPIbCMXVFxKi
OFqPfvYQqFXaATTFnuGEUAm8TqXlBHFfNJIBwBSqirqeY7HN8K9CK5bDKZUY8ysO
dXBBqxp0uMPIANZZe8O7I2FzRVblbS2D2w==
-----END CERTIFICATE-----
)EOF";

// ─────────────────────────────────────────────

void MqttManager::begin(Actuators* actuators) {
  _instance  = this;
  _actuators = actuators;

  // TLS sin verificación de certificado (para pruebas)
  // Una vez confirmado que conecta, reemplazar por setCACert()
  _wifiClient.setInsecure();
  _wifiClient.setTimeout(5);  // timeout 5 segundos

  _client.setServer(MQTT_HOST, MQTT_PORT);
  _client.setCallback(_callback);
  _client.setBufferSize(512);

  _syncNTP();
  _reconnect();
}

// ─── NTP ─────────────────────────────────────
void MqttManager::_syncNTP() {
  configTime(NTP_GMT_OFFSET, NTP_DST_OFFSET, NTP_SERVER1, NTP_SERVER2);
  Serial.print("[NTP] Sincronizando");
  struct tm ti;
  int tries = 0;
  while (!getLocalTime(&ti) && tries < 20) {
    delay(500);
    Serial.print(".");
    tries++;
  }
  if (tries < 20) {
    char buf[32];
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &ti);
    Serial.printf("\n[NTP] ✅ Hora: %s\n", buf);
  } else {
    Serial.println("\n[NTP] ⚠️  No se pudo sincronizar");
  }
}

// ─── Reconnect ───────────────────────────────
void MqttManager::_reconnect() {
  int retries = 0;
  while (!_client.connected() && retries < 5) {
    Serial.printf("[MQTT] Conectando a %s ... ", MQTT_HOST);
    if (_client.connect(MQTT_CLIENT_ID, MQTT_USER, MQTT_PASS)) {
      Serial.println("✅ Conectado");
      _client.subscribe(TOPIC_CMD_BOMBA);
      _client.subscribe(TOPIC_CMD_LED);
      _client.subscribe(TOPIC_CMD_VENTILADOR);
      _client.subscribe(TOPIC_CMD_MODO);
      _client.subscribe(TOPIC_CMD_UMBRALES);
    } else {
      Serial.printf("❌ rc=%d — reintento en 3s\n", _client.state());
      delay(3000);
      retries++;
    }
  }
}

// ─── Loop ────────────────────────────────────
void MqttManager::loop() {
  if (!_client.connected()) _reconnect();
  _client.loop();
}

// ─── Publish sensores ────────────────────────
void MqttManager::publishSensors(const SensorData& d) {
  if (!_client.connected()) return;

  // Obtener timestamp
  struct tm ti;
  char ts[25] = "sin-ntp";
  if (getLocalTime(&ti)) strftime(ts, sizeof(ts), "%Y-%m-%dT%H:%M:%S", &ti);

  // Temperatura
  {
    StaticJsonDocument<128> doc;
    doc["valor"]     = d.dhtOk ? d.temperatura : (float)-999;
    doc["unidad"]    = "°C";
    doc["timestamp"] = ts;
    char buf[128];
    serializeJson(doc, buf);
    _client.publish(TOPIC_TEMPERATURA, buf);
  }

  // Humedad
  {
    StaticJsonDocument<128> doc;
    doc["valor"]     = d.dhtOk ? d.humedad : (float)-999;
    doc["unidad"]    = "%";
    doc["timestamp"] = ts;
    char buf[128];
    serializeJson(doc, buf);
    _client.publish(TOPIC_HUMEDAD, buf);
  }

  // Luz (valor ADC + porcentaje)
  {
    StaticJsonDocument<128> doc;
    doc["valor"]      = d.luz;
    doc["porcentaje"] = (int)((d.luz / 4095.0f) * 100);
    doc["timestamp"]  = ts;
    char buf[128];
    serializeJson(doc, buf);
    _client.publish(TOPIC_LUZ, buf);
  }
}

// ─── Publish status / healthcheck ────────────
void MqttManager::publishStatus(const SensorData& d) {
  if (!_client.connected()) return;

  struct tm ti;
  char ts[25] = "sin-ntp";
  if (getLocalTime(&ti)) strftime(ts, sizeof(ts), "%Y-%m-%dT%H:%M:%S", &ti);

  StaticJsonDocument<256> doc;
  doc["status"]       = "online";
  doc["modo"]         = _manual ? "manual" : "auto";
  doc["timestamp"]    = ts;
  doc["uptime_s"]     = millis() / 1000;
  doc["bomba"]        = _actuators->getBomba();
  doc["led"]          = _actuators->getLed();
  doc["ventilador"]   = _actuators->getVentilador();
  doc["ip"]           = WiFi.localIP().toString();

  char buf[256];
  serializeJson(doc, buf);
  _client.publish(TOPIC_STATUS, buf, true);  // retained
}

// ─── Callback estático ───────────────────────
void MqttManager::_callback(char* topic, byte* payload, unsigned int length) {
  String t(topic);
  String p;
  for (unsigned int i = 0; i < length; i++) p += (char)payload[i];
  if (_instance) _instance->_handleMessage(t, p);
}

// ─── Manejar mensajes entrantes ──────────────
void MqttManager::_handleMessage(const String& topic, const String& payload) {
  Serial.printf("[MQTT] ← %s : %s\n", topic.c_str(), payload.c_str());

  if (topic == TOPIC_CMD_MODO) {
    _manual = (payload == "manual");
    Serial.printf("[MQTT] Modo → %s\n", _manual ? "MANUAL" : "AUTO");
    return;
  }

  if (topic == TOPIC_CMD_UMBRALES) {
    StaticJsonDocument<128> doc;
    if (!deserializeJson(doc, payload)) {
      if (doc.containsKey("tempMax"))    _umbrales.tempMax    = doc["tempMax"].as<float>();
      if (doc.containsKey("humedadMin")) _umbrales.humedadMin = doc["humedadMin"].as<float>();
      if (doc.containsKey("luzMin"))     _umbrales.luzMin     = doc["luzMin"].as<int>();
      Serial.printf("[MQTT] Umbrales → T>%.1f | H<%.1f | L<%d\n",
        _umbrales.tempMax, _umbrales.humedadMin, _umbrales.luzMin);
    }
    return;
  }

  // Comandos manuales solo se procesan en modo manual
  if (!_manual) {
    Serial.println("[MQTT] ⚠️  Comando ignorado (modo AUTO)");
    return;
  }

  bool estado = (payload == "1" || payload == "true" || payload == "ON");

  if (topic == TOPIC_CMD_BOMBA)      _actuators->setBomba(estado);
  if (topic == TOPIC_CMD_LED)        _actuators->setLed(estado);
  if (topic == TOPIC_CMD_VENTILADOR) _actuators->setVentilador(estado);
}
