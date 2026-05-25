# 🌿 Invernadero IoT — ESP32

Sistema de monitoreo y control automatizado de invernadero basado en ESP32, con comunicación MQTT cifrada (TLS), dashboard web local y sincronización de tiempo NTP.

---

## 👥 Integrantes

- Juan Camilo Gómez Bayona
- Daniel Sanchez Sotelo
- Jeronimo Infante Vega

---

## 📋 Tabla de contenido

- [Descripción general](#descripción-general)
- [Hardware utilizado](#hardware-utilizado)
- [Diagrama de flujo](#diagrama-de-flujo)
- [Endpoints API REST](#endpoints-api-rest)
- [Temas MQTT](#temas-mqtt)
- [Librerías utilizadas](#librerías-utilizadas)
- [Uso de memoria](#uso-de-memoria)
- [Limitaciones](#limitaciones)
- [Posibilidades de mejora](#posibilidades-de-mejora)

---

## Descripción general

El sistema permite monitorear en tiempo real la temperatura, humedad y luminosidad de un invernadero, y controlar automáticamente (o manualmente desde un dashboard web) una bomba de agua, un LED de iluminación y un ventilador.

Toda la comunicación con el broker MQTT viaja cifrada por TLS en el puerto 8883 usando HiveMQ Cloud. El tiempo del ESP32 se sincroniza al arrancar mediante NTP.

---

## Hardware utilizado

| Componente | Descripción |
|---|---|
| ESP32 Dev Module | Microcontrolador principal |
| DHT22 | Sensor de temperatura y humedad |
| Fotoresistencia (LDR) | Sensor de luminosidad analógico |
| Módulo Relay x4 | Control de actuadores |
| Mini bomba sumergible 5V | Actuador de riego |
| LED 5V | Actuador de iluminación |
| Ventilador 5V | Actuador de ventilación |
| Fuente 5V / 3A | Alimentación de actuadores y ESP32 |

---

## Diagrama de flujo

```
┌─────────────────────────────────────────────────────┐
│                    ARRANQUE ESP32                    │
└─────────────────────┬───────────────────────────────┘
                      │
                      ▼
         ┌────────────────────────┐
         │  ¿Hay credenciales     │
         │    WiFi guardadas?     │
         └────────┬───────────────┘
          NO      │        SÍ
          ▼       │        ▼
    Portal        │   Conectar WiFi
    captivo       │        │
    AP Setup      │        ▼
                  │   Sincronizar NTP
                  │        │
                  │        ▼
                  │   Conectar MQTT TLS
                  │   (HiveMQ Cloud :8883)
                  │        │
                  │        ▼
                  │   Iniciar WebServer
                  │   (Dashboard :80)
                  │
                  └─────────────────────┐
                                        │
              ┌─────────────────────────▼──────────────────────┐
              │                     LOOP                        │
              │                                                 │
              │  Cada 5 s:                                      │
              │  ┌──────────────────────────────────────────┐  │
              │  │ Leer DHT22 → temperatura, humedad        │  │
              │  │ Leer LDR   → luminosidad (ADC 12 bit)   │  │
              │  │ Publicar datos → MQTT broker             │  │
              │  └──────────────────────────────────────────┘  │
              │                                                 │
              │  ¿Modo AUTO?                                    │
              │  ┌──────────────────────────────────────────┐  │
              │  │ Temp > umbral máx  → Ventilador ON       │  │
              │  │ Humedad < umbral mín → Bomba ON          │  │
              │  │ Luz < umbral mín   → LED ON              │  │
              │  └──────────────────────────────────────────┘  │
              │                                                 │
              │  ¿Modo MANUAL?                                  │
              │  ┌──────────────────────────────────────────┐  │
              │  │ Comandos desde Dashboard o MQTT          │  │
              │  │ controlan directamente los actuadores    │  │
              │  └──────────────────────────────────────────┘  │
              │                                                 │
              │  Cada 30 s: Publicar healthcheck → MQTT        │
              └─────────────────────────────────────────────────┘
```

---

## Endpoints API REST

El dashboard web corre en el ESP32 en el puerto 80. Todos los endpoints retornan JSON.

---

### `GET /`

Retorna el HTML del dashboard de control.

**Respuesta:** `text/html` — Página web del invernadero.

---

### `GET /api/status`

Healthcheck del sistema. Verifica que el servidor está arriba y retorna el estado general.

**Respuesta:**
```json
{
  "status": "ok",
  "uptime_s": 3600,
  "timestamp": "2026-05-24T22:48:32",
  "modo": "auto",
  "bomba": false,
  "led": false,
  "ventilador": true,
  "ip": "10.210.225.116"
}
```

---

### `GET /api/sensors`

Retorna la lectura actual de todos los sensores.

**Respuesta:**
```json
{
  "temperatura": 27.5,
  "humedad": 65.0,
  "luzRaw": 1024,
  "luzPct": 25,
  "dhtOk": true
}
```

| Campo | Tipo | Descripción |
|---|---|---|
| `temperatura` | float | Temperatura en °C. `-999` si el DHT22 falla |
| `humedad` | float | Humedad relativa en %. `-999` si el DHT22 falla |
| `luzRaw` | int | Valor ADC crudo (0 – 4095) |
| `luzPct` | int | Luminosidad en porcentaje (0 – 100%) |
| `dhtOk` | bool | `false` si la lectura del DHT22 es inválida |

---

### `GET /api/actuators`

Retorna el estado actual de los actuadores.

**Respuesta:**
```json
{
  "bomba": false,
  "led": true,
  "ventilador": false
}
```

---

### `POST /api/control`

Cambia el modo de operación o controla un actuador en modo manual.

**Body:**
```json
{
  "accion": "modo",
  "valor": "manual"
}
```

| `accion` | `valor` válidos | Descripción |
|---|---|---|
| `"modo"` | `"auto"` / `"manual"` | Cambia el modo de operación |
| `"bomba"` | `"1"` / `"0"` | Enciende o apaga la bomba (solo en modo manual) |
| `"led"` | `"1"` / `"0"` | Enciende o apaga el LED (solo en modo manual) |
| `"ventilador"` | `"1"` / `"0"` | Enciende o apaga el ventilador (solo en modo manual) |

**Respuesta:**
```json
{ "ok": true }
```

---

### `POST /api/umbrales`

Actualiza los umbrales del modo automático en tiempo real.

**Body:**
```json
{
  "tempMax": 30.0,
  "humedadMin": 40.0,
  "luzMin": 300
}
```

| Campo | Tipo | Descripción |
|---|---|---|
| `tempMax` | float | Temperatura máxima en °C — activa el ventilador |
| `humedadMin` | float | Humedad mínima en % — activa la bomba |
| `luzMin` | int | Valor ADC mínimo (0-4095) — activa el LED |

Todos los campos son opcionales; solo se actualizan los que se envíen.

**Respuesta:**
```json
{ "ok": true }
```

---

## Temas MQTT

Broker: **HiveMQ Cloud** — Puerto **8883 (TLS)**

### Publica (ESP32 → Broker)

| Topic | Frecuencia | Payload de ejemplo |
|---|---|---|
| `invernadero/sensores/temperatura` | Cada 5 s | `{"valor": 27.5, "unidad": "°C", "timestamp": "2026-05-24T22:48:32"}` |
| `invernadero/sensores/humedad` | Cada 5 s | `{"valor": 65.0, "unidad": "%", "timestamp": "2026-05-24T22:48:32"}` |
| `invernadero/sensores/luz` | Cada 5 s | `{"valor": 1024, "porcentaje": 25, "timestamp": "2026-05-24T22:48:32"}` |
| `invernadero/status` | Cada 30 s | `{"status": "online", "modo": "auto", "bomba": false, "led": false, "ventilador": true, "ip": "10.x.x.x", "uptime_s": 3600, "timestamp": "..."}` |

El topic `invernadero/status` se publica con **retained = true**, por lo que cualquier cliente que se suscriba recibe el último estado inmediatamente.

---

### Suscribe (Broker → ESP32)

| Topic | Payload válido | Descripción |
|---|---|---|
| `invernadero/cmd/modo` | `"auto"` / `"manual"` | Cambia el modo de operación |
| `invernadero/cmd/bomba` | `"1"` / `"0"` / `"ON"` / `"OFF"` | Controla la bomba (solo en modo manual) |
| `invernadero/cmd/led` | `"1"` / `"0"` / `"ON"` / `"OFF"` | Controla el LED (solo en modo manual) |
| `invernadero/cmd/ventilador` | `"1"` / `"0"` / `"ON"` / `"OFF"` | Controla el ventilador (solo en modo manual) |
| `invernadero/cmd/umbrales` | JSON con `tempMax`, `humedadMin`, `luzMin` | Actualiza umbrales automáticos |

> Los comandos a actuadores enviados por MQTT son ignorados si el sistema está en modo AUTO.

---

## Librerías utilizadas

| Librería | Versión recomendada | Autor | Uso |
|---|---|---|---|
| `DHT sensor library` | ≥ 1.4.4 | Adafruit | Lectura del sensor DHT22 |
| `Adafruit Unified Sensor` | ≥ 1.1.9 | Adafruit | Dependencia de DHT |
| `PubSubClient` | ≥ 2.8.0 | Nick O'Leary | Cliente MQTT |
| `ArduinoJson` | ≥ 6.21.0 | Benoit Blanchon | Serialización/deserialización JSON |
| `WiFi` | (incluida en ESP32 core) | Espressif | Conexión WiFi |
| `WiFiClientSecure` | (incluida en ESP32 core) | Espressif | TLS sobre WiFi |
| `WebServer` | (incluida en ESP32 core) | Espressif | Servidor HTTP |
| `DNSServer` | (incluida en ESP32 core) | Espressif | Portal captivo DNS |
| `Preferences` | (incluida en ESP32 core) | Espressif | Almacenamiento NVS (credenciales WiFi) |

---

## Uso de memoria

Generado por Arduino IDE al verificar/cargar el sketch:

```
El Sketch usa 1134297 bytes (86%) del espacio de almacenamiento de programa.
El máximo es 1310720 bytes.

Las variables globales usan 49252 bytes (15%) de la memoria dinámica,
dejando 278428 bytes para las variables locales.
El máximo es 327680 bytes.
```

| Recurso | Usado | Total | Porcentaje |
|---|---|---|---|
| Flash (programa) | 1.134.297 bytes | 1.310.720 bytes | 86% |
| RAM (variables globales) | 49.252 bytes | 327.680 bytes | 15% |

> El uso alto de Flash se debe principalmente al HTML del dashboard y los certificados TLS embebidos. La RAM disponible para variables locales (278 KB) es suficiente para la operación normal.

---

## Limitaciones

- **Un solo cliente web a la vez:** El `WebServer` del ESP32 es de un solo hilo; conexiones simultáneas pueden causar lentitud.
- **Sin persistencia de datos:** Las lecturas de sensores no se almacenan localmente; solo se publican al broker MQTT. Si se pierde la conexión, los datos de ese período se pierden.
- **Flash al 86%:** Queda poco espacio para agregar funcionalidades grandes como HTTPS en el servidor web o más lógica de control.
- **Sin reconexión automática al WiFi:** Si el router se reinicia, el ESP32 necesita reiniciarse manualmente para reconectarse.
- **Verificación de certificado TLS desactivada:** La conexión MQTT cifra el tráfico pero no verifica la identidad del broker (`setInsecure()`).
- **Sin autenticación en el dashboard:** Cualquier dispositivo en la misma red puede acceder y controlar el invernadero.
- **LDR sin calibración:** El valor de luminosidad es relativo al divisor de voltaje construido; no está calibrado en lux.

---

## Posibilidades de mejora

- **Reconexión WiFi automática:** Detectar pérdida de conexión en el loop y reconectar sin reiniciar.
- **Certificado TLS correcto:** Cargar el certificado raíz del broker HiveMQ para activar la verificación completa.
- **Autenticación en el dashboard:** Agregar un login básico con usuario y contraseña para proteger el acceso.
- **Almacenamiento local de datos:** Usar la memoria SPIFFS/LittleFS del ESP32 para guardar un historial de lecturas.
- **Gráficas históricas:** Mostrar en el dashboard una gráfica de temperatura/humedad de las últimas horas.
- **Notificaciones:** Enviar alertas por MQTT cuando un sensor sale del rango normal.
- **OTA (Over The Air):** Actualizar el firmware del ESP32 por WiFi sin necesidad de conectar el cable USB.
- **Sensor de nivel de agua:** Agregar un sensor para saber cuándo el depósito de la bomba está vacío.
- **Múltiples zonas:** Escalar el sistema para controlar varias secciones del invernadero de forma independiente.
- **Integración con Node-RED o Grafana:** Conectar el broker MQTT a una plataforma de visualización más completa.
