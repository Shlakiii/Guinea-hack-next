#include <core/input/button_input.h>
#include <core/config/buttons.h>

namespace {
constexpr uint32_t debounce_ms = 35;
constexpr uint8_t button_count = 4;
}

void ButtonInput::begin() {
    const uint8_t pins[button_count] = {
        BTN_UP,
        BTN_DOWN,
        BTN_SELECT,
        BTN_BACK
    };
    const uint32_t now = millis();

    for (uint8_t i = 0; i < button_count; ++i) {
        buttons_[i].pin = pins[i];
        pinMode(buttons_[i].pin, INPUT_PULLUP);
        buttons_[i].raw_pressed = digitalRead(buttons_[i].pin) == LOW;
        buttons_[i].stable_pressed = buttons_[i].raw_pressed;
        buttons_[i].changed_at = now;
    }
}

ButtonEvents ButtonInput::update() {
    ButtonEvents events = {};
    const uint32_t now = millis();

    for (uint8_t i = 0; i < button_count; ++i) {
        ButtonState& button = buttons_[i];
        const bool pressed = digitalRead(button.pin) == LOW;
        if (pressed != button.raw_pressed) {
            button.raw_pressed = pressed;
            button.changed_at = now;
        }

        if (button.stable_pressed != button.raw_pressed &&
            now - button.changed_at >= debounce_ms) {
            button.stable_pressed = button.raw_pressed;
            if (button.stable_pressed) {
                switch (i) {
                    case 0: events.up = true; break;
                    case 1: events.down = true; break;
                    case 2: events.select = true; break;
                    case 3: events.back = true; break;
                }
            }
        }
    }

    return events;
}
