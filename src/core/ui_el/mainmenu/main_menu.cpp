#include <core/ui_el/mainmenu/main_menu.h>
#include <core/ui/uimain.h>
#include <esp32-hal-rgb-led.h>
#include <core/output/buzzer.h>

namespace {
constexpr uint8_t menu_item_count = sizeof(btns) / sizeof(btns[0]);
constexpr uint8_t flashlight_item = 2;

uint8_t selected_item = 0;
bool flashlight_active = false;

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

    beep(3);
}
}

void main_menu_init(LGFX& display) {
    rgbLedWrite(RGB_BUILTIN, 0, 0, 0);
    flashlight_active = false;
    selected_item = 0;
    draw_menu(display);
}

MainMenuEvent main_menu_update(LGFX& display, const ButtonEvents& input) {
    if (flashlight_active) {
        if (input.back) {
            rgbLedWrite(RGB_BUILTIN, 0, 0, 0);
            flashlight_active = false;
            draw_menu(display);
        }
        return MainMenuEvent::None;
    }

    bool selection_changed = false;
    if (input.up) {
        selected_item = (selected_item + menu_item_count - 1) % menu_item_count;
        selection_changed = true;
        beep(1);
    } else if (input.down) {
        selected_item = (selected_item + 1) % menu_item_count;
        selection_changed = true;
        beep(1);
    }

    if (selection_changed) {
        draw_menu(display);
    }

    if (input.select) {
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
    if (input.back) {
        return MainMenuEvent::Back;
    }
    return MainMenuEvent::None;
}