#ifndef MENU_MAIN_H
#define MENU_MAIN_H

#include <Arduino.h>
#include <core/config/display.h>
#include <core/input/button_input.h>
#include <core/types/graphics.h>
#include <core/assets/icons/mainMenuIcons.h>

static const btnsTR btns[] = {
    { // el:WiFi?btn
        {
            bitmap_wifi_24,
            24,24
        },
        236,26,
        2,2,
        "WiFi"
    },

    { // el:Bluetooth?btn
        {
            bitmap_bluetooth_24,
            24,24
        },
        236,26,
        2,30,
        "Bluetooth"
    },

    {
        {
            bitmap_flashlight,
            24,24
        },
        236,26,
        2,58,
        "Flashlight"
    }
};

enum class MainMenuEvent : uint8_t {
    None,
    Select,
    OpenWifi,
    Back
};

void main_menu_init(LGFX& display);
MainMenuEvent main_menu_update(LGFX& display, const ButtonEvents& input);

#endif