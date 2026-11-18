#include "uimain.h"

static void drawMonochromeIcon(
  LGFX& display,
  const IconBitmap& icon,
  int16_t x,
  int16_t y,
  uint16_t color
) {
  const int16_t bytes_per_row = (icon.width + 7) / 8;
  for (int16_t iy = 0; iy < icon.height; ++iy) {
    for (int16_t ix = 0; ix < icon.width; ++ix) {
      const uint8_t byte = icon.pixels[iy * bytes_per_row + ix / 8];
      if ((byte & (0x80u >> (ix & 7))) == 0) {
        display.drawPixel(x + ix, y + iy, color);
      }
    }
  }
}

void labelWithIco(
  LGFX& display,
  const char* text,
  int16_t x, int16_t y,
  int16_t w, int16_t h,
  IconBitmap icon,
  bool selected
) {
  constexpr int16_t padding = 6;
  constexpr int16_t icon_gap = 8;
  const uint16_t background = selected ? 0x2104 : 0x0000;
  const uint16_t foreground = 0xFFFF;
  const uint16_t border = selected ? 0x07FF : 0x7BEF;

  display.fillRoundRect(x, y, w, h, 6, background);
  display.drawRoundRect(x, y, w, h, 6, border);

  const bool has_icon =
    icon.pixels != nullptr && icon.width > 0 && icon.height > 0;
  int16_t text_size = h < 36 ? 1 : 2;
  display.setTextSize(text_size);
  if (has_icon &&
      icon.width + padding + icon_gap + display.textWidth(text) > w) {
    text_size = 1;
    display.setTextSize(text_size);
  }

  int16_t text_x;
  if (has_icon) {
    const int16_t icon_x = x + padding;
    const int16_t icon_y = y + (h - icon.height) / 2;
    drawMonochromeIcon(display, icon, icon_x, icon_y, foreground);
    text_x = icon_x + icon.width + icon_gap;
  } else {
    text_x = x + (w - display.textWidth(text)) / 2;
  }

  const int16_t text_y = y + (h - 8 * text_size) / 2;
  display.setTextColor(foreground, background);
  display.setCursor(text_x, text_y);
  display.print(text);
}
