#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <stdint.h>
#include <Arduino.h>

struct IconBitmap {
    const uint8_t* pixels;
    int16_t width;
    int16_t height;
};

struct btnsTR {
    IconBitmap icon;
    int16_t width; int16_t height;
    int16_t posx; int16_t posy;
    String text;
};


#endif