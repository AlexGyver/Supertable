
/*
    1 клик - движение к дальней. Если движется - остановить
    2 клик - запомнить 1
    3 клик - запомнить 2
    4 клик - сброс нуля

    Удерж - движение в последнем направлении
    1 клик + удерж - едем вверх
    2 клик + удерж - едем вниз
*/

#include <Arduino.h>
#include <Blinker.h>
#include <EEManager.h>
#include <EncButton.h>

#include "controller.h"

#define BTN_PIN 4
#define BTN_LED 3
#define TACH_PIN 2
#define MOTOR_1 9
#define MOTOR_2 10
#define MOT_SPEED 255

Blinker blinker(BTN_LED);
Button btn(BTN_PIN);
Controller<DRIVER2WIRE_PWM> ctrl(TACH_PIN, MOTOR_1, MOTOR_2);

struct Data {
    int16_t pos = 0;
    int16_t pos1 = 0;
    int16_t pos2 = 0;
};
Data data;

EEManager mem(data);

void setup() {
    mem.begin(0, 'a');
    ctrl.setPos(data.pos);
    ctrl.setSpeed(MOT_SPEED);

    // Пины D9 и D10 - 7.8 кГц
    TCCR1A = 0b00000001;  // 8bit
    TCCR1B = 0b00001010;  // x8 fast pwm
}

void loop() {
    blinker.tick();
    ctrl.tick();

    if (mem.tick()) {
        blinker.blink(1, 100, 100);
    }

    if (ctrl.stopped()) {
        data.pos = ctrl.getPos();
        mem.update();
    }

    if (btn.tick()) {
        if (btn.hasClicks()) {
            switch (btn.clicks) {
                case 1:
                    if (ctrl.running()) {
                        ctrl.stop();
                    } else {
                        int d1 = abs(data.pos1 - ctrl.getPos());
                        int d2 = abs(data.pos2 - ctrl.getPos());
                        ctrl.runTarget(d1 > d2 ? data.pos1 : data.pos2);
                        mem.stop();
                    }
                    break;

                case 2:
                    data.pos1 = ctrl.getPos();
                    mem.update();
                    blinker.blink(2, 200, 200);
                    break;

                case 3:
                    data.pos2 = ctrl.getPos();
                    mem.update();
                    blinker.blink(3, 200, 200);
                    break;

                case 4:
                    ctrl.setPos(0);
                    ctrl.stop();
                    blinker.blink(4, 200, 200);
                    break;
            }
        }

        if (btn.hold()) mem.stop();
        if (btn.hold(0)) ctrl.runSpeed();
        if (btn.hold(1)) ctrl.runSpeed(1);
        if (btn.hold(2)) ctrl.runSpeed(-1);
        if (btn.releaseHoldStep()) ctrl.stop();
    }
}