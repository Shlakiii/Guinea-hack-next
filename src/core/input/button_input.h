#ifndef BUTTON_INPUT_H
#define BUTTON_INPUT_H

#include <Arduino.h>

struct ButtonEvents {
    bool up;
    bool down;
    bool select;
    bool back;
};

class ButtonInput {
public:
    void begin();
    ButtonEvents update();

private:
    struct ButtonState {
        uint8_t pin;
        bool raw_pressed;
        bool stable_pressed;
        uint32_t changed_at;
    };

    ButtonState buttons_[4] = {};
};

#endif
