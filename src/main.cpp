#include <Arduino.h>
#include <core/input/button_input.h>
#include <core/ui_el/mainmenu/main_menu.h>
#include <core/ui_el/wifi/wifi_menu.h>

LGFX display;
ButtonInput button_input;

namespace {
bool wifi_menu_active = false;
}

void setup() {
  display.init();
  button_input.begin();
  main_menu_init(display);
}

void loop() {
  const ButtonEvents input = button_input.update();

  if (wifi_menu_active) {
    if (wifi_menu_update(display, input) == WifiMenuEvent::Back) {
      wifi_menu_active = false;
      main_menu_init(display);
    }
    return;
  }

  if (main_menu_update(display, input) == MainMenuEvent::OpenWifi) {
    wifi_menu_active = true;
    wifi_menu_init(display);
  }
}