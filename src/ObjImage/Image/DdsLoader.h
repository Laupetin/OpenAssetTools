#pragma once

#include "Image/Texture.h"

#include <istream>
#include <memory>

namespace dds
{
    std::unique_ptr<Texture> LoadDds(std::istream& stream);
}
