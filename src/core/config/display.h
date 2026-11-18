#ifndef DISPLAY_H
#define DISPLAY_H

#define LGFX_USE_V1
#include <LovyanGFX.hpp>

#define TFT_DC    5
#define TFT_CS    10
#define TFT_SCL   12
#define TFT_SDA   11
#define TFT_RESET 4

class LGFX : public lgfx::LGFX_Device {
  lgfx::Panel_ST7789 _panel;
  lgfx::Bus_SPI _bus;

public:
  LGFX() {
    auto bus_cfg = _bus.config();
    bus_cfg.spi_host = SPI2_HOST;
    bus_cfg.spi_mode = 0;
    bus_cfg.freq_write = 40000000;
    bus_cfg.freq_read = 16000000;
    bus_cfg.spi_3wire = false;
    bus_cfg.use_lock = true;
    bus_cfg.dma_channel = SPI_DMA_CH_AUTO;
    bus_cfg.pin_sclk = TFT_SCL;
    bus_cfg.pin_mosi = TFT_SDA;
    bus_cfg.pin_miso = -1;
    bus_cfg.pin_dc = TFT_DC;
    _bus.config(bus_cfg);
    _panel.setBus(&_bus);

    auto panel_cfg = _panel.config();
    panel_cfg.pin_cs = TFT_CS;
    panel_cfg.pin_rst = TFT_RESET;
    panel_cfg.pin_busy = -1;
    panel_cfg.panel_width = 240;
    panel_cfg.panel_height = 320;
    panel_cfg.offset_x = 0;
    panel_cfg.offset_y = 0;
    panel_cfg.invert = true;
    panel_cfg.readable = false;
    _panel.config(panel_cfg);

    setPanel(&_panel);
  }
};

#endif // DISPLAY_H