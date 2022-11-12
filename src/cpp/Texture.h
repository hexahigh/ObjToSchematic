#pragma once

#include <string>
#include <vector>
#include <stdint.h>

class RGBA;

class Texture
{
public:
    Texture();
    ~Texture();

    bool Load(const std::string& file_path);

    RGBA GetColour(const float u, const float v) const;

private:
    RGBA GetColour(const uint32_t x, const uint32_t y) const;

private:
    uint8_t* data_;
    uint32_t width_;
    uint32_t height_;
    uint32_t channels_;
};