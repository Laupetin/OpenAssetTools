#pragma once

#include "Image/Texture.h"

#include <istream>
#include <memory>

namespace iwi
{
    std::unique_ptr<Texture> LoadIwi(std::istream& stream);
}; // namespace iwi
