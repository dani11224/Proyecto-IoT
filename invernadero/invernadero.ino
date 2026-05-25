/*
 * ╔══════════════════════════════════════════════════════╗
 * ║         INVERNADERO IoT — ESP32                      ║
 * ║  Sensores : DHT22 (temp/hum) + LDR (luz)            ║
 * ║  Actuadores: Bomba · LED · Ventilador (relay x4)    ║
 * ║  Comunicación: MQTT TLS → HiveMQ Cloud              ║
 * ║  Dashboard : WebServer local (puerto 80)             ║
 * ╚══════════════════════════════════════════════════════╝
 *
 *  Librerías necesarias (instalar desde Library Manager):
 *    - DHT sensor library  (Adafruit)
 *    - Adafruit Unified Sensor
 *    - PubSubClient         (Nick O'Leary)
 *    - ArduinoJson          (Benoit Blanchon)  >= v6
 */

#include "config.h"
#include "wifi_manager.h"
#include "sensors.h"
#include "actuators.h"
#include "mqtt_manager.h"
#include "web_server.h"

// ─── Objetos globales ────────────────────────
WifiManager     wifiMgr;
Sensors         sensors;
Actuators       actuators;
MqttManager     mqttMgr;
WebServerManager webSrv;

// ─── Timers ──────────────────────────────────
unsigned long lastSensorPublish  = 0;
unsigned long lastHealthcheck    = 0;

// ════════════════════════════════════════════
void setup() {
  Serial.begin(115200);
  Serial.println("\n🌿 Invernadero IoT — Iniciando...");

  // 1. Conectar WiFi (o iniciar portal captivo)
  bool connected = wifiMgr.begin();

  // 2. Inicializar sensores y actuadores
  sensors.begin();
  actuators.begin();

  if (connected) {
    // 3. MQTT + NTP  (solo con WiFi)
    mqttMgr.begin(&actuators);

    // 4. Healthcheck inicial
    SensorData d = sensors.read();
    mqttMgr.publishStatus(d);
  }

  // 5. Servidor web (dashboard o portal captivo)
  webSrv.begin(&sensors, &actuators, &mqttMgr, wifiMgr.isConfigMode());

  Serial.println("✅ Setup completo");
  if (connected) {
    Serial.printf("🌐 Dashboard: http://%s\n", WiFi.localIP().toString().c_str());
  } else {
    Serial.printf("📡 Portal captivo: http://%s\n", WiFi.softAPIP().toString().c_str());
  }
}

// ════════════════════════════════════════════
void loop() {
  // Botón reset (mantener 3 s)
  wifiMgr.checkResetButton();

  // DNS en modo AP
  if (wifiMgr.isConfigMode()) {
    wifiMgr.loopDNS();
    webSrv.loop();
    return;   // no ejecutar lógica de sensores sin WiFi
  }

  // ── MQTT keep-alive ──
  mqttMgr.loop();
  yield();

  // ── Web server ──
  webSrv.loop();
  yield();

  unsigned long now = millis();

  // ── Leer sensores y publicar ──────────────
  if (now - lastSensorPublish >= INTERVALO_SENSORES) {
    lastSensorPublish = now;

    SensorData d = sensors.read();

    // Publicar al broker
    mqttMgr.publishSensors(d);

    // ── Lógica automática (solo en modo AUTO) ──
    if (!mqttMgr.modoManual()) {
      Umbrales& u = mqttMgr.umbrales();

      // Ventilador: temperatura > máximo
      if (d.dhtOk) {
        actuators.setVentilador(d.temperatura > u.tempMax);
        actuators.setBomba(d.humedad < u.humedadMin);
      }

      // LED: luminosidad baja
      actuators.setLed(d.luz < u.luzMin);
    }
  }

  // ── Healthcheck periódico ─────────────────
  if (now - lastHealthcheck >= INTERVALO_HEALTHCHECK) {
    lastHealthcheck = now;
    SensorData d = sensors.read();
    mqttMgr.publishStatus(d);
  }
}
