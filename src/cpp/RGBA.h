#pragma once

#include <stdint.h>

class RGBA
{
public:
    RGBA(uint8_t in_r, uint8_t in_g, uint8_t in_b, uint8_t in_a)
        : r(in_r), g(in_g), b(in_b), a(in_a)
    {
    }

public:
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
};