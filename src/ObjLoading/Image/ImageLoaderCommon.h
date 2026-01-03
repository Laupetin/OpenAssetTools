#pragma once

#include "Asset/AssetCreationResult.h"
#include "Image/IwiTypes.h"
#include "Image/Texture.h"

#include <memory>
#include <optional>
#include <string>

namespace image
{
    struct CommonImageLoaderResult
    {
        std::optional<AssetCreationResult> GetResultIfCancelled() const;

        bool m_failure;
        std::string m_iwi_path;
        std::unique_ptr<Texture> m_texture;
    };

    CommonImageLoaderResult LoadImageCommon();
} // namespace image
