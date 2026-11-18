#pragma once

#include <Arduino.h>
#include "../config/display.h"
#include "../types/graphics.h"

void labelWithIco(
  LGFX& display,
  const char* text,
  int16_t x, int16_t y,
  int16_t w, int16_t h,
  IconBitmap icon,
  bool selected = false
);