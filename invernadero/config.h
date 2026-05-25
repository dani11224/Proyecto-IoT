#pragma once

// ─────────────────────────────────────────────
//  CREDENCIALES WIFI  (se sobreescriben en runtime via portal captivo)
// ─────────────────────────────────────────────
#define WIFI_AP_NAME     "Invernadero_Setup"
#define RESET_PIN        4          // GPIO botón reset (3 s = borrar config)

// ─────────────────────────────────────────────
//  BROKER MQTT — HiveMQ Cloud (TLS 8883)
//  Crea tu cluster gratis en: https://console.hivemq.cloud
//  y reemplaza estos valores
// ─────────────────────────────────────────────
#define MQTT_HOST        "7ba72639998d4e869180efe2e0e9ea62.s1.eu.hivemq.cloud"
#define MQTT_PORT        8883
#define MQTT_USER        "camilo"
#define MQTT_PASS        "Camilo123"
#define MQTT_CLIENT_ID   "ESP32_Invernadero"

// Topics  publish  (ESP32 → broker)
#define TOPIC_TEMPERATURA    "invernadero/sensores/temperatura"
#define TOPIC_HUMEDAD        "invernadero/sensores/humedad"
#define TOPIC_LUZ            "invernadero/sensores/luz"
#define TOPIC_STATUS         "invernadero/status"

// Topics  subscribe  (broker → ESP32)
#define TOPIC_CMD_BOMBA      "invernadero/cmd/bomba"
#define TOPIC_CMD_LED        "invernadero/cmd/led"
#define TOPIC_CMD_VENTILADOR "invernadero/cmd/ventilador"
#define TOPIC_CMD_MODO       "invernadero/cmd/modo"       // "auto" | "manual"
#define TOPIC_CMD_UMBRALES   "invernadero/cmd/umbrales"   // JSON con rangos

// ─────────────────────────────────────────────
//  NTP
// ─────────────────────────────────────────────
#define NTP_SERVER1      "pool.ntp.org"
#define NTP_SERVER2      "time.nist.gov"
#define NTP_GMT_OFFSET   -18000      // UTC-5  (Colombia)
#define NTP_DST_OFFSET   0

// ─────────────────────────────────────────────
//  PINES SENSORES
// ─────────────────────────────────────────────
#define PIN_DHT          14          // DHT22 data
#define PIN_LDR          34          // Fotoresistencia (ADC1)

// ─────────────────────────────────────────────
//  PINES RELAY  (activo en LOW para la mayoría de módulos de 4 canales)
// ─────────────────────────────────────────────
#define PIN_RELAY_BOMBA       26
#define PIN_RELAY_LED         27
#define PIN_RELAY_VENTILADOR  25
#define RELAY_ON              LOW
#define RELAY_OFF             HIGH

// ─────────────────────────────────────────────
//  UMBRALES POR DEFECTO
// ─────────────────────────────────────────────
#define UMBRAL_TEMP_MAX      30.0f   // °C  → enciende ventilador
#define UMBRAL_HUMEDAD_MIN   40.0f   // %   → enciende bomba
#define UMBRAL_LUZ_MIN       300     // 0-4095 (ADC 12 bit) → enciende LED

// ─────────────────────────────────────────────
//  INTERVALOS  (ms)
// ─────────────────────────────────────────────
#define INTERVALO_SENSORES   5000    // cada 5 s lee sensores
#define INTERVALO_MQTT_PUB   5000    // cada 5 s publica al broker
#define INTERVALO_HEALTHCHECK 30000  // cada 30 s publica status
