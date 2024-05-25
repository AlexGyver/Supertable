#pragma once
#include <Arduino.h>
#include <GyverMotor2.h>

#include "slot.h"

template <GM_driver GM_TYPE = DRIVER2WIRE_PWM, uint8_t GM_RES = 8>
class Controller {
   public:
    enum class State {
        Idle,
        RunSpeed,
        RunTarget,
    };

    Controller(uint8_t slotPin, uint8_t mpin1, uint8_t mpin2, uint8_t mpin3 = 255) : slot(slotPin), motor(mpin1, mpin2, mpin3) {
        motor.setSmoothSpeed(10);
    }

    void tick() {
        motor.tick();
        if (slot.tick(motor.getState()) && _state == State::RunTarget) {
            if ((motor.getState() == 1 && slot.pos >= _target) ||
                (motor.getState() == -1 && slot.pos <= _target)) {
                stop();
            }
        }
    }

    bool stopped() {
        return _stopF ? _stopF = false, true : false;
    }

    void setSpeed(uint16_t speed) {
        _speed = speed;
    }

    void runSpeed() {
        runSpeed(_lastDir);
    }

    void runSpeed(int8_t dir) {
        motor.smoothMode(true);
        motor.setSpeed(_speed * dir);
        _lastDir = dir;
        _state = State::RunSpeed;
    }

    void runTarget(int16_t target) {
        if (slot.pos == target) return;
        _target = target;
        runSpeed(slot.pos < target ? 1 : -1);
        _state = State::RunTarget;
    }

    void stop() {
        motor.smoothMode(false);
        motor.stop();
        _state = State::Idle;
        _stopF = true;
    }

    void setPos(int16_t pos) {
        slot.pos = pos;
    }

    int16_t getPos() {
        return slot.pos;
    }

    State getState() {
        return _state;
    }

    bool running() {
        return _state != State::Idle;
    }

    Slot slot;
    GMotor2<GM_TYPE, GM_RES> motor;

   private:
    State _state = State::Idle;
    int16_t _target = 0;
    uint8_t _speed = 255;
    int8_t _lastDir = 1;
    bool _stopF = 0;
};