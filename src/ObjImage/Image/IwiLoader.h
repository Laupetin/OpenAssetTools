#pragma once

#include "Image/IwiTypes.h"
#include "Image/Texture.h"

#include <istream>
#include <memory>
#include <optional>

namespace image
{
    struct IwiLoaderResult
    {
        IwiVersion m_version;
        CommonIwiMetaData m_meta;
        std::unique_ptr<Texture> m_texture;
    };

    std::optional<IwiLoaderResult> LoadIwi(std::istream& stream);
}; // namespace image
