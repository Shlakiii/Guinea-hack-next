#include <core/ui_el/mainmenu/main_menu.h>
#include <core/ui/uimain.h>
#include <esp32-hal-rgb-led.h>

namespace {
constexpr uint32_t debounce_ms = 35;
constexpr uint8_t menu_item_count = sizeof(btns) / sizeof(btns[0]);
constexpr uint8_t flashlight_item = 2;

struct ButtonState {
    uint8_t pin;
    bool raw_pressed;
    bool stable_pressed;
    uint32_t changed_at;
};

ButtonState buttons[] = {
    {BTN_UP, false, false, 0},
    {BTN_DOWN, false, false, 0},
    {BTN_SELECT, false, false, 0},
    {BTN_BACK, false, false, 0}
};

uint8_t selected_item = 0;
bool flashlight_active = false;

bool wasPressed(ButtonState& button, uint32_t now) {
    const bool pressed = digitalRead(button.pin) == LOW;
    if (pressed != button.raw_pressed) {
        button.raw_pressed = pressed;
        button.changed_at = now;
    }

    if (button.stable_pressed != button.raw_pressed &&
        now - button.changed_at >= debounce_ms) {
        button.stable_pressed = button.raw_pressed;
        return button.stable_pressed;
    }
    return false;
}

void draw_menu(LGFX& display) {
    display.fillScreen(TFT_BLACK);

    for (uint8_t i = 0; i < menu_item_count; ++i) {
        const btnsTR& button = btns[i];
        labelWithIco(
            display,
            button.text.c_str(),
            button.posx,
            button.posy,
            button.width,
            button.height,
            button.icon,
            i == selected_item
        );
    }
}

void draw_flashlight_screen(LGFX& display) {
    display.fillScreen(TFT_BLACK);
    display.setTextColor(TFT_WHITE, TFT_BLACK);
    display.setTextSize(2);
    display.setCursor(44, 130);
    display.print("Flashlight ON");
    display.setTextSize(1);
    display.setCursor(76, 162);
    display.print("BACK to return");
}
}

void main_menu_init(LGFX& display) {
    for (ButtonState& button : buttons) {
        pinMode(button.pin, INPUT_PULLUP);
        button.raw_pressed = digitalRead(button.pin) == LOW;
        button.stable_pressed = button.raw_pressed;
        button.changed_at = millis();
    }
    rgbLedWrite(RGB_BUILTIN, 0, 0, 0);
    flashlight_active = false;
    selected_item = 0;
    draw_menu(display);
}

MainMenuEvent main_menu_update(LGFX& display) {
    const uint32_t now = millis();
    const bool up = wasPressed(buttons[0], now);
    const bool down = wasPressed(buttons[1], now);
    const bool select = wasPressed(buttons[2], now);
    const bool back = wasPressed(buttons[3], now);

    if (flashlight_active) {
        if (back) {
            rgbLedWrite(RGB_BUILTIN, 0, 0, 0);
            flashlight_active = false;
            draw_menu(display);
        }
        return MainMenuEvent::None;
    }

    bool selection_changed = false;
    if (up) {
        selected_item = (selected_item + menu_item_count - 1) % menu_item_count;
        selection_changed = true;
    } else if (down) {
        selected_item = (selected_item + 1) % menu_item_count;
        selection_changed = true;
    }

    if (selection_changed) {
        draw_menu(display);
    }

    if (select) {
        if (selected_item == flashlight_item) {
            rgbLedWrite(RGB_BUILTIN, 255, 255, 255);
            flashlight_active = true;
            draw_flashlight_screen(display);
            return MainMenuEvent::None;
        }
        if (selected_item == 0) {
            return MainMenuEvent::OpenWifi;
        }
        return MainMenuEvent::Select;
    }
    if (back) {
        return MainMenuEvent::Back;
    }
    return MainMenuEvent::None;
}