#pragma once

#include "Image/Texture.h"

#include <istream>
#include <memory>

namespace image
{
    std::unique_ptr<Texture> LoadIwi(std::istream& stream);
}; // namespace image
