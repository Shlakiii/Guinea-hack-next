#include <Arduino.h>
#include <core/ui_el/mainmenu/main_menu.h>
#include <core/ui_el/wifi/wifi_menu.h>

LGFX display;

namespace {
bool wifi_menu_active = false;
}

void setup() {
  display.init();
  main_menu_init(display);
}

void loop() {
  if (wifi_menu_active) {
    if (wifi_menu_update(display) == WifiMenuEvent::Back) {
      wifi_menu_active = false;
      main_menu_init(display);
    }
    return;
  }

  if (main_menu_update(display) == MainMenuEvent::OpenWifi) {
    wifi_menu_active = true;
    wifi_menu_init(display);
  }
}