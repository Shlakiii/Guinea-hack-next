#include <Arduino.h>
#include <WiFi.h>
#include <core/input/button_input.h>
#include <core/ui/uimain.h>
#include <core/ui_el/wifi/wifi_menu.h>
#include <core/types/graphics.h>
#include <core/assets/icons/mainMenuIcons.h>
#include <core/output/buzzer.h>

namespace {
constexpr int16_t scan_row_y = 24;
constexpr int16_t scan_row_height = 30;
constexpr int16_t results_y = 28;
constexpr int16_t result_row_height = 41;
constexpr int16_t visible_networks =
    (320 - results_y) / result_row_height;

enum class ScanState : uint8_t {
    Idle,
    Scanning,
    Complete,
    Failed
};

enum class WifiView : uint8_t {
    Scan,
    Results
};

ScanState scan_state = ScanState::Idle;
WifiView current_view = WifiView::Scan;
int16_t network_count = 0;
int16_t selected_item = 0;
int16_t selected_scan_action = 0;

const char* security_name(wifi_auth_mode_t security) {
    switch (security) {
        case WIFI_AUTH_OPEN: return "OPEN";
        case WIFI_AUTH_WEP: return "WEP";
        case WIFI_AUTH_WPA_PSK: return "WPA";
        case WIFI_AUTH_WPA2_PSK: return "WPA2";
        case WIFI_AUTH_WPA_WPA2_PSK: return "WPA/WPA2";
        case WIFI_AUTH_WPA2_ENTERPRISE: return "WPA2 ENT";
        case WIFI_AUTH_WPA3_PSK: return "WPA3";
        case WIFI_AUTH_WPA2_WPA3_PSK: return "WPA2/3";
        default: return "SECURE";
    }
}

void draw_network_row(LGFX& display, int16_t network_index, int16_t y,
                      bool selected) {
    const uint16_t background = selected ? 0x2104 : TFT_BLACK;
    const uint16_t foreground = TFT_WHITE;
    display.fillRect(2, y, 236, result_row_height - 1, background);
    if (selected) {
        display.drawRect(2, y, 236, result_row_height - 1, 0x07FF);
    }

    const int16_t icon_x = 7;
    const int16_t icon_y = y + (result_row_height - 16) / 2;
    constexpr int16_t icon_bytes_per_row = 2;
    for (int16_t iy = 0; iy < 16; ++iy) {
        for (int16_t ix = 0; ix < 16; ++ix) {
            const uint8_t byte =
                bitmap_wifi_16[iy * icon_bytes_per_row + ix / 8];
            if ((byte & (0x80u >> (ix & 7))) == 0) {
                display.drawPixel(icon_x + ix, icon_y + iy, foreground);
            }
        }
    }

    String ssid = WiFi.SSID(network_index);
    if (ssid.isEmpty()) {
        ssid = "<hidden>";
    }

    display.setTextSize(1);
    display.setTextColor(foreground, background);
    display.setCursor(30, y + 5);
    display.print(ssid.substring(0, 34));
    display.setCursor(30, y + 22);
    display.printf("%d dBm  ", WiFi.RSSI(network_index));
    display.print(security_name(WiFi.encryptionType(network_index)));
}

void draw_scan_menu(LGFX& display) {
    display.fillScreen(TFT_BLACK);
    display.setTextColor(TFT_WHITE, TFT_BLACK);
    display.setTextSize(1);
    display.setCursor(6, 8);
    display.print("WiFi networks");

    labelWithIco(
        display,
        "Scan",
        2,
        scan_row_y,
        236,
        scan_row_height,
        {bitmap_find_24, 24, 24},
        selected_scan_action == 0
    );

    if (scan_state == ScanState::Complete) {
        labelWithIco(
            display,
            "Results",
            2,
            58,
            236,
            scan_row_height,
            {bitmap_wifi_16, 16, 16},
            selected_scan_action == 1
        );
    }

    display.setTextSize(1);
    display.setTextColor(TFT_WHITE, TFT_BLACK);
    display.setCursor(6, 94);
    switch (scan_state) {
        case ScanState::Idle:
            display.print("SELECT: scan   BACK: menu");
            break;
        case ScanState::Scanning:
            display.print("Scanning...");
            break;
        case ScanState::Complete:
            beep(1);
            delay(3);
            display.printf("Found networks: %d", network_count);
            break;
        case ScanState::Failed:
            beep(2);
            delay(4);
            display.print("Scan failed. Try again.");
            break;
    }
}

void draw_results_menu(LGFX& display) {
    display.fillScreen(TFT_BLACK);
    display.setTextColor(TFT_WHITE, TFT_BLACK);
    display.setTextSize(1);
    display.setCursor(6, 8);
    display.printf("WiFi results: %d  BACK: menu SELECT: act", network_count);

    if (network_count == 0) {
        display.setCursor(6, 36);
        display.print("No networks found");
        return;
    }

    int16_t first_network = 0;
    if (selected_item >= visible_networks) {
        first_network = selected_item - visible_networks + 1;
    }

    for (int16_t row = 0; row < visible_networks; ++row) {
        const int16_t network_index = first_network + row;
        if (network_index >= network_count) {
            break;
        }
        draw_network_row(
            display,
            network_index,
            results_y + row * result_row_height,
            selected_item == network_index
        );
    }

    beep(2);
}

void draw_current_view(LGFX& display) {
    if (current_view == WifiView::Results) {
        draw_results_menu(display);
    } else {
        draw_scan_menu(display);
    }
}

void start_scan() {
    WiFi.scanDelete();
    WiFi.mode(WIFI_STA);
    WiFi.scanNetworks(true, false);
    scan_state = ScanState::Scanning;
    network_count = 0;
    selected_item = 0;
    current_view = WifiView::Scan;

    beep(2);
}
}

void wifi_menu_init(LGFX& display) {
    const int16_t scan_result = WiFi.scanComplete();
    if (scan_result == WIFI_SCAN_RUNNING) {
        scan_state = ScanState::Scanning;
    } else if (scan_result >= 0) {
        scan_state = ScanState::Complete;
        network_count = scan_result;
        current_view = WifiView::Results;
        selected_scan_action = 1;
    } else {
        scan_state = ScanState::Idle;
        network_count = 0;
        current_view = WifiView::Scan;
    }
    selected_item = 0;
    if (scan_state != ScanState::Complete) {
        selected_scan_action = 0;
    }
    draw_current_view(display);
}

WifiMenuEvent wifi_menu_update(LGFX& display, const ButtonEvents& input) {
    if (input.back) {
        if (current_view == WifiView::Results) {
            current_view = WifiView::Scan;
            draw_current_view(display);
            return WifiMenuEvent::None;
        }
        return WifiMenuEvent::Back;
    }

    if (scan_state == ScanState::Scanning) {
        const int16_t result = WiFi.scanComplete();
        if (result >= 0) {
            network_count = result;
            scan_state = ScanState::Complete;
            current_view = WifiView::Results;
            selected_item = 0;
            selected_scan_action = 1;
            draw_current_view(display);
        } else if (result != WIFI_SCAN_RUNNING) {
            scan_state = ScanState::Failed;
            draw_current_view(display);
        }
        return WifiMenuEvent::None;
    }

    bool changed = false;
    if (current_view == WifiView::Results && network_count > 0) {
        if (input.up) {
            selected_item =
                (selected_item + network_count - 1) % network_count;
            changed = true;
        } else if (input.down) {
            selected_item = (selected_item + 1) % network_count;
            changed = true;
        }
    } else if (current_view == WifiView::Scan) {
        const int16_t action_count =
            scan_state == ScanState::Complete ? 2 : 1;
        if (input.up && action_count > 1) {
            selected_scan_action =
                (selected_scan_action + action_count - 1) % action_count;
            changed = true;
        } else if (input.down && action_count > 1) {
            selected_scan_action = (selected_scan_action + 1) % action_count;
            changed = true;
        }

        if (input.select && selected_scan_action == 0) {
            start_scan();
            changed = true;
        } else if (input.select && selected_scan_action == 1 &&
                   scan_state == ScanState::Complete) {
            current_view = WifiView::Results;
            selected_item = 0;
            changed = true;
        }
    }

    if (changed) {
        draw_current_view(display);
    }
    return WifiMenuEvent::None;
}
