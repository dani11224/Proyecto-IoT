#include "actuators.h"

void Actuators::begin() {
  pinMode(PIN_RELAY_BOMBA,       OUTPUT);
  pinMode(PIN_RELAY_LED,         OUTPUT);
  pinMode(PIN_RELAY_VENTILADOR,  OUTPUT);

  // Apagar todo al iniciar (relay activo-bajo → HIGH = apagado)
  _relay(PIN_RELAY_BOMBA,      false);
  _relay(PIN_RELAY_LED,        false);
  _relay(PIN_RELAY_VENTILADOR, false);

  Serial.println("[Actuators] ✅ Relays inicializados (OFF)");
}

void Actuators::_relay(uint8_t pin, bool on) {
  digitalWrite(pin, on ? RELAY_ON : RELAY_OFF);
}

void Actuators::setBomba(bool on) {
  if (_bomba == on) return;
  _bomba = on;
  _relay(PIN_RELAY_BOMBA, on);
  Serial.printf("[Actuators] 💧 Bomba → %s\n", on ? "ON" : "OFF");
}

void Actuators::setLed(bool on) {
  if (_led == on) return;
  _led = on;
  _relay(PIN_RELAY_LED, on);
  Serial.printf("[Actuators] 💡 LED  → %s\n", on ? "ON" : "OFF");
}

void Actuators::setVentilador(bool on) {
  if (_ventilador == on) return;
  _ventilador = on;
  _relay(PIN_RELAY_VENTILADOR, on);
  Serial.printf("[Actuators] 🌀 Vent → %s\n", on ? "ON" : "OFF");
}
