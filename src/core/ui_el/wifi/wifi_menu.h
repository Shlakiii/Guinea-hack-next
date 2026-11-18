#ifndef WIFI_MENU_H
#define WIFI_MENU_H

#include <Arduino.h>
#include <core/config/display.h>

enum class WifiMenuEvent : uint8_t {
    None,
    Back
};

void wifi_menu_init(LGFX& display);
WifiMenuEvent wifi_menu_update(LGFX& display);

#endif
