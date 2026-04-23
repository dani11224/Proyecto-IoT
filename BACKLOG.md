# 📋 Backlog — RecycleBot

> Este archivo define el backlog inicial del proyecto. Las tareas deben migrarse al **GitHub Projects** del repositorio usando las etiquetas definidas abajo.

## Etiquetas / Labels

| Label | Color | Descripción |
|-------|-------|-------------|
| `must-have` | 🔴 Rojo | Esencial para el MVP |
| `nice-to-have` | 🟡 Amarillo | Mejora opcional |
| `spike` | 🟣 Morado | Investigación técnica de viabilidad |
| `hardware` | 🟤 Café | Relacionado con ESP32 / físico |
| `backend` | 🔵 Azul | API, BD, servidor |
| `frontend` | 🟢 Verde | App móvil / dashboard web |
| `ia` | 🟠 Naranja | Inteligencia artificial / ML |
| `infra` | ⚫ Negro | DevOps, hosting, configuración |

---

## 🟣 SPIKE — Release 1

| # | Tarea | Labels | Asignado | Sprint |
|---|-------|--------|----------|--------|
| S1 | Investigar y probar ESP32-CAM: captura de imagen y envío HTTP al servidor | `spike` `hardware` | — | Sprint 2 |
| S2 | Probar clasificación de materiales reciclables con Google Vision API o modelo TFLite | `spike` `ia` | — | Sprint 2 |
| S3 | Medir latencia del ciclo completo (captura → clasificación → respuesta) | `spike` `hardware` `ia` | — | Sprint 2 |
| S4 | Documentar resultado del spike: tasa de acierto, latencia, decisión arquitectónica | `spike` | — | Sprint 2 |

---

## 🔴 Must-Have Features

### Hardware / ESP32
| # | Tarea | Labels | Sprint |
|---|-------|--------|--------|
| H1 | Configurar entorno de desarrollo ESP32 (PlatformIO/Arduino IDE) | `must-have` `hardware` | Sprint 1 |
| H2 | Implementar captura de imagen con ESP32-CAM | `must-have` `hardware` | Sprint 2 |
| H3 | Implementar envío de imagen vía HTTP POST al backend | `must-have` `hardware` | Sprint 2 |
| H4 | Integrar sensor de peso HX711 para registrar gramos del material | `must-have` `hardware` | Sprint 5 |
| H5 | Feedback visual al usuario (LEDs o pantalla TFT) según resultado de clasificación | `must-have` `hardware` | Sprint 4 |
| H6 | Sensor IR de presencia para detectar objeto depositado | `must-have` `hardware` | Sprint 3 |

### Backend
| # | Tarea | Labels | Sprint |
|---|-------|--------|--------|
| B1 | Setup proyecto backend (FastAPI o Node.js) con estructura base | `must-have` `backend` | Sprint 3 |
| B2 | Configurar Supabase: tablas usuarios, transacciones, materiales, puntos | `must-have` `backend` `infra` | Sprint 3 |
| B3 | Endpoint POST `/classify` — recibe imagen, llama a IA, devuelve categoría | `must-have` `backend` `ia` | Sprint 3 |
| B4 | Endpoint POST `/transaction` — registra reciclaje y calcula puntos | `must-have` `backend` | Sprint 3 |
| B5 | Sistema de autenticación de usuarios (Supabase Auth / JWT) | `must-have` `backend` | Sprint 4 |
| B6 | Endpoint GET `/user/{id}/history` — historial de reciclaje | `must-have` `backend` | Sprint 4 |
| B7 | Endpoint GET `/user/{id}/points` — saldo de puntos actual | `must-have` `backend` | Sprint 4 |

### IA / Clasificación
| # | Tarea | Labels | Sprint |
|---|-------|--------|--------|
| I1 | Integrar modelo de clasificación de materiales (Google Vision API o modelo custom) | `must-have` `ia` | Sprint 3 |
| I2 | Definir y documentar categorías: plástico, vidrio, cartón, metal, no-reciclable | `must-have` `ia` | Sprint 2 |
| I3 | Dataset inicial de imágenes de materiales reciclables para pruebas | `must-have` `ia` | Sprint 2 |

### Frontend / App
| # | Tarea | Labels | Sprint |
|---|-------|--------|--------|
| F1 | Setup proyecto app (React Native o Flutter) | `must-have` `frontend` | Sprint 4 |
| F2 | Pantalla de registro e inicio de sesión | `must-have` `frontend` | Sprint 4 |
| F3 | Pantalla de saldo de puntos y resumen | `must-have` `frontend` | Sprint 4 |
| F4 | Pantalla de historial de reciclaje | `must-have` `frontend` | Sprint 4 |
| F5 | Dashboard admin básico (ver transacciones de la máquina) | `must-have` `frontend` | Sprint 5 |

---

## 🟡 Nice-to-Have Features

### Gamificación y Engagement
| # | Tarea | Labels | Sprint |
|---|-------|--------|--------|
| N1 | Sistema de misiones diarias/semanales con recordatorios push | `nice-to-have` `backend` `frontend` | Sprint 5 |
| N2 | Niveles de usuario (bronce, plata, oro) según volumen reciclado | `nice-to-have` `frontend` | Sprint 5 |
| N3 | Leaderboard de recicladores en la misma zona | `nice-to-have` `backend` `frontend` | Sprint 6 |
| N4 | Badges / logros desbloqueables | `nice-to-have` `frontend` | Sprint 6 |

### Integración Supermercados (B2B)
| # | Tarea | Labels | Sprint |
|---|-------|--------|--------|
| N5 | Motor de recomendaciones de cupones personalizados | `nice-to-have` `ia` `backend` | Sprint 6 |
| N6 | Sugerencias de productos reciclables en próxima compra | `nice-to-have` `ia` `frontend` | Sprint 6 |
| N7 | Reporte ESG exportable (PDF/CSV) para cadenas B2B | `nice-to-have` `backend` `frontend` | Sprint 6 |

### Infraestructura y Escalabilidad
| # | Tarea | Labels | Sprint |
|---|-------|--------|--------|
| N8 | Soporte multi-máquina (registro y gestión de múltiples kioscos) | `nice-to-have` `backend` `infra` | Sprint 6 |
| N9 | Sistema de alertas cuando la máquina está llena | `nice-to-have` `hardware` `backend` | Sprint 6 |

---

## 📦 Definición de Done (DoD)

Una tarea se considera **terminada** cuando:
- [ ] El código está en la rama correspondiente y hace merge a `main` vía PR revisado.
- [ ] Tiene al menos un test básico o evidencia de prueba manual documentada.
- [ ] La documentación relevante está actualizada (README, comentarios en código).
- [ ] El issue en GitHub Projects está movido a **Done**.
