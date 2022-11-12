#include "Texture.h"
#include "RGBA.h"

#define STB_IMAGE_IMPLEMENTATION
#include "./external/stb_image.h"

Texture::Texture()
{
}

Texture::~Texture()
{
    stbi_image_free(this->data_);
}

bool Texture::Load(const std::string& file_path)
{
    int width, height, channels;
    unsigned char *image = stbi_load(file_path.c_str(), &width, &height, &channels, 0);
    if(image == nullptr)
    {
        printf("Error in loading the image\n");
        return false;
    }

    assert(channels == 3 || channels == 4);

    this->width_ = width;
    this->height_ = height;
    this->channels_ = channels;
    this->data_ = image;

    return true;
}

RGBA Texture::GetColour(const uint32_t x, const uint32_t y) const
{
    assert(0 <= x && x < this->width_);
    assert(0 <= y && y < this->height_);

    const size_t base_index = this->channels_ * (y * this->width_ + x);
    const uint8_t r = this->data_[base_index + 0];
    const uint8_t g = this->data_[base_index + 1];
    const uint8_t b = this->data_[base_index + 2];
    const uint8_t a = (channels_ == 4) ? this->data_[base_index + 3] : 255;

    return RGBA(r, g, b, a);
}

RGBA Texture::GetColour(const float u, const float v) const
{
    const uint32_t x = u * (this->width_ - 1);
    const uint32_t y = v * (this->height_ - 1);

    return this->GetColour(x, y);
}