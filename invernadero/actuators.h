#pragma once
#include <Arduino.h>
#include "config.h"

class Actuators {
public:
  void begin();

  void setBomba(bool on);
  void setLed(bool on);
  void setVentilador(bool on);

  bool getBomba()      const { return _bomba; }
  bool getLed()        const { return _led; }
  bool getVentilador() const { return _ventilador; }

private:
  bool _bomba      = false;
  bool _led        = false;
  bool _ventilador = false;

  void _relay(uint8_t pin, bool on);
};
