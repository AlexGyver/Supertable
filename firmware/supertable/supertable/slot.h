#pragma once
#include <Arduino.h>
#include <GyverIO.h>

#define DEB_PRD 20

class Slot {
   public:
    Slot(uint8_t pin) : _pin(pin) {
        _prev = gio::read(_pin);
    }

    bool tick(int8_t dir) {
        bool cur = gio::read(_pin);
        if (_prev != cur && millis() - _tmr >= DEB_PRD) {
            _tmr = millis();
            _prev = cur;
            if (cur) {
                pos += dir;
                return 1;
            }
        }
        return 0;
    }

    int16_t pos;

   private:
    uint8_t _pin;
    bool _prev;
    uint32_t _tmr;
};