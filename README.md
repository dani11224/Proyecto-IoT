# ♻️ RecycleBot — Recicla. Gana. Impacta.

> **Máquina inteligente de reciclaje con sistema de recompensas, potenciada por IA y conectada a supermercados colombianos.**

---

## 🌎 Visión del Proyecto

### ¿Qué problema resuelve?

Colombia genera más de **12 millones de toneladas de residuos sólidos al año**, pero solo recicla formalmente el **17%** de ese total. La paradoja: millones de colombianos ya separan sus residuos en casa, pero cuando salen a la calle **no encuentran a dónde llevarlos**. Los puntos de reciclaje son escasos, invisibles o inseguros. Los supermercados —el punto de contacto más frecuente entre ciudadano y empaque plástico— no tienen ningún sistema de retorno.

**La falla no es de conciencia ciudadana. La falla es de infraestructura y de incentivos.**

### ¿Cómo lo resolvemos?

RecycleBot es un **sistema físico-digital** compuesto por:
- Una **máquina receptora de reciclaje** (kiosco) instalada en supermercados, con clasificación automática mediante IA y hardware ESP32.
- Una **aplicación móvil** que gestiona puntos, misiones, recordatorios y cupones de descuento.
- Un **backend** que conecta el comportamiento de reciclaje del usuario con su historial de compras (alianza con cadenas).

### ¿A quiénes salva?

| Segmento | Perfil |
|----------|--------|
| **B2C** | Personas de 18–55 años, residentes urbanos/periurbanos en Colombia, con smartphone, que ya reciclan en casa pero no tienen a dónde llevar los residuos. Compran en Éxito, Jumbo, D1, Ara o Carulla. |
| **B2B** | Cadenas de supermercados (Grupo Éxito, Jumbo, Carulla) que necesitan cumplir metas ESG, diferenciarse con atributos verdes y fidelizar clientes. |

---

## ⚙️ Arquitectura del Sistema

### Diagrama de Bloques

```
┌─────────────────────────────────────────────────────────────────┐
│                        USUARIO FINAL                            │
│              (deposita material reciclable)                     │
└────────────────────────────┬────────────────────────────────────┘
                             │
                             ▼
┌─────────────────────────────────────────────────────────────────┐
│                    KIOSCO FÍSICO (Hardware)                      │
│                                                                  │
│  ┌─────────────┐   ┌──────────────┐   ┌──────────────────────┐ │
│  │  Cámara     │──▶│   ESP32-CAM  │──▶│  Sensor de peso /    │ │
│  │  (imagen)   │   │  + WiFi      │   │  Sensor IR           │ │
│  └─────────────┘   └──────┬───────┘   └──────────────────────┘ │
│                            │                                     │
│                    MQTT / HTTP REST                              │
└────────────────────────────┬────────────────────────────────────┘
                             │
                             ▼
┌─────────────────────────────────────────────────────────────────┐
│                        BACKEND (Cloud)                           │
│                                                                  │
│  ┌──────────────────┐    ┌───────────────────────────────────┐  │
│  │  API REST        │    │  Motor de IA                      │  │
│  │  (Node.js /      │◀──▶│  - Clasificación de materiales    │  │
│  │   FastAPI)       │    │    (Vision API / modelo propio)   │  │
│  └────────┬─────────┘    │  - Análisis de patrones usuario   │  │
│           │              │  - Motor de recomendaciones       │  │
│           ▼              └───────────────────────────────────┘  │
│  ┌──────────────────┐    ┌───────────────────────────────────┐  │
│  │  Base de Datos   │    │  Motor de Puntos & Recompensas    │  │
│  │  (Supabase /     │◀──▶│  - Cálculo de puntos por material │  │
│  │   PostgreSQL)    │    │  - Canje de cupones               │  │
│  └──────────────────┘    └───────────────────────────────────┘  │
└────────────────────────────┬────────────────────────────────────┘
                             │
                   ┌─────────┴──────────┐
                   │                    │
                   ▼                    ▼
        ┌─────────────────┐   ┌──────────────────────┐
        │  App Móvil      │   │  Dashboard B2B       │
        │  (React Native/ │   │  (Reportes ESG,      │
        │   Flutter)      │   │   inventario,        │
        │                 │   │   datos reciclaje)   │
        │  - Historial    │   └──────────────────────┘
        │  - Puntos       │
        │  - Misiones     │
        │  - Cupones      │
        └─────────────────┘
```

### Stack Tecnológico

| Capa | Tecnología |
|------|-----------|
| **Hardware** | ESP32-CAM (clasificación visual), ESP32 (control general), sensor de peso HX711, sensor IR de presencia |
| **Firmware** | Arduino IDE / PlatformIO (C++) |
| **Comunicación** | MQTT sobre WiFi (ESP32 → Backend) |
| **Backend** | FastAPI (Python) o Node.js + Express |
| **IA/ML** | TensorFlow Lite (on-device en ESP32 o en servidor), Google Vision API (como respaldo) |
| **Base de datos** | Supabase (PostgreSQL + Auth + Storage) |
| **App Móvil** | React Native o Flutter |
| **Dashboard B2B** | React.js |

---

## 🔧 Restricciones de Hardware — ESP32

| Recurso | Valor | Implicación |
|---------|-------|-------------|
| **RAM** | 520 KB SRAM | El modelo de IA no puede correr completamente on-device; la imagen se envía al servidor para clasificación |
| **Flash** | 4 MB (típico) | Firmware liviano; assets mínimos almacenados localmente |
| **CPU** | Xtensa LX6 dual-core @ 240 MHz | Suficiente para captura de imagen, MQTT y lógica básica |
| **Consumo** | ~240 mA activo / ~10 µA deep sleep | Requiere fuente de alimentación estable (kiosco conectado a red eléctrica) |
| **Conectividad** | WiFi 802.11 b/g/n, Bluetooth | WiFi principal; BT como respaldo local |
| **Cámara (ESP32-CAM)** | OV2640, hasta 2MP | Resolución suficiente para clasificar plástico, vidrio, cartón |
| **Temperatura operación** | –40°C a +85°C | Apto para interior de supermercado |

**Decisión arquitectónica clave:** La clasificación de imágenes se realiza **en el servidor** (no on-device), ya que la RAM del ESP32 es insuficiente para modelos de visión robustos. El ESP32-CAM captura la imagen → la envía vía HTTP POST al backend → el backend responde con la categoría → el ESP32 actúa (abre compuerta, muestra resultado en pantalla/LEDs).

---

## 💰 Presupuesto Estimado (Prototipo MVP)

| Componente | Costo estimado (COP) |
|------------|----------------------|
| ESP32-CAM | $35.000 |
| ESP32 DevKit (control) | $25.000 |
| Sensor de peso HX711 + celda de carga | $20.000 |
| Sensor IR de presencia | $8.000 |
| Pantalla TFT 2.4" (feedback visual) | $35.000 |
| Estructura física kiosco (acrílico/madera) | $200.000 |
| Fuente de alimentación 5V/3A | $18.000 |
| Cables, protoboard, conectores | $15.000 |
| Hosting backend (Supabase free tier + VPS básico) | $0 – $50.000/mes |
| **Total hardware prototipo** | **~$356.000** |

---

## 🚀 MVP — Minimum Viable Product

El MVP demuestra el ciclo completo: **depositar un objeto → clasificarlo con IA → sumar puntos al usuario**.

### Must-Have (esencial para funcionar)
- [ ] ESP32-CAM captura imagen del objeto depositado
- [ ] Backend recibe imagen y devuelve categoría (plástico / vidrio / cartón / metal / otro)
- [ ] Registro de usuario y autenticación (app/web básica)
- [ ] Sistema de puntos: asignación automática según material y peso
- [ ] Historial de reciclaje del usuario
- [ ] Dashboard básico de administración (ver transacciones de la máquina)

### Nice-to-Have (mejoras futuras)
- [ ] Sistema de misiones y recordatorios push
- [ ] Integración con historial de compras de supermercados
- [ ] Motor de recomendaciones de cupones personalizado
- [ ] Reporte ESG exportable para B2B
- [ ] Soporte multi-máquina (red de kioscos)
- [ ] Gamificación avanzada (niveles, badges, leaderboard)

---

## 🔬 Spike Arquitectónico — Release 1

**Pregunta crítica:** ¿Es viable clasificar materiales reciclables (plástico, vidrio, cartón, metal) con suficiente precisión usando la cámara del ESP32-CAM enviando imágenes a un modelo en servidor?

**Tareas del Spike:**
1. Configurar ESP32-CAM para capturar y enviar imagen vía HTTP a un servidor local.
2. Probar Google Vision API (o un modelo TFLite en servidor) con imágenes de materiales reciclables comunes.
3. Medir latencia total (captura → clasificación → respuesta) y evaluar si es aceptable (< 3 segundos).
4. Documentar tasa de acierto con al menos 20 objetos de prueba.

**Criterio de éxito:** Clasificación correcta ≥ 75% en condiciones de iluminación de supermercado, latencia < 3s.

---

## 📅 Cronograma — Sprints y Releases

### Release 1 — Fundamentos y Viabilidad (Semanas 1–2)
| Sprint | Tareas |
|--------|--------|
| **Sprint 1** (Sem 1) | Setup repo, arquitectura, README, backlog en GitHub Projects, configuración entorno ESP32 |
| **Sprint 2** (Sem 2) | **SPIKE**: ESP32-CAM → servidor → clasificación IA. Primer prototipo de comunicación funcional |

**Entregable:** Certeza técnica de viabilidad + repo documentado + backlog priorizado.

### Release 2 — Núcleo Funcional (Semanas 3–4)
| Sprint | Tareas |
|--------|--------|
| **Sprint 3** (Sem 3) | Backend: API REST, base de datos Supabase, endpoint de clasificación |
| **Sprint 4** (Sem 4) | Auth de usuario, sistema de puntos, historial básico en app/web |

**Entregable:** Ciclo completo funcionando: depositar → clasificar → puntuar → ver en app.

### Release 3 — Experiencia Completa (Semanas 5–6)
| Sprint | Tareas |
|--------|--------|
| **Sprint 5** (Sem 5) | Misiones, recordatorios, sistema de cupones básico |
| **Sprint 6** (Sem 6) | Dashboard B2B, integración sensor de peso, mejora UX app |

**Entregable:** Producto con todas las features must-have funcionales y primera iteración de nice-to-have.

### Release 4 — Pulido y Entrega Final (Semanas 7–8 + Finales)
| Sprint | Tareas |
|--------|--------|
| **Sprint 7** (Sem 7) | Feature freeze. Corrección de bugs, estabilización del sistema |
| **Sprint 8** (Sem 8) | Preparación demo/pitch, documentación final, ajustes presentación |

**Entregable:** Producto final listo para demostración.

---

## 🧠 ¿Qué tareas "sobrehumanas" hará la IA?

1. **Clasificación visual** de materiales reciclables al momento del depósito.
2. **Análisis de patrones de comportamiento** del usuario (frecuencia, materiales, horarios).
3. **Recordatorios inteligentes** basados en historial de compras y reciclaje.
4. **Recomendaciones personalizadas** de cupones según preferencias de consumo.
5. **Sugerencias de compra** indicando qué productos de la próxima compra son reciclables.

---

## 👥 Equipo

>Daniel Sanchez Sotelo, Juan Camilo Gomez Bayona, Jeronimo Infante Vega

---

## 📄 Licencia

MIT License — Ver `LICENSE` para más detalles.
