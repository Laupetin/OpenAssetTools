#pragma once

#include "Asset/AssetCreationResult.h"
#include "Image/IwiTypes.h"
#include "Image/Texture.h"
#include "SearchPath/ISearchPath.h"

#include <cstdint>
#include <memory>
#include <optional>
#include <string>

namespace image
{
    enum class CommonImageLoaderHashType
    {
        NONE,
        CRC32
    };

    union CommonImageLoaderHash
    {
        uint32_t crc32;
    };

    struct CommonImageLoaderResult
    {
        std::optional<AssetCreationResult> GetResultIfCancelled() const;

        bool m_failure;
        size_t m_iwi_size;
        CommonIwiMetaData m_meta;
        std::unique_ptr<Texture> m_texture;
        CommonImageLoaderHash m_hash;
    };

    CommonImageLoaderResult
        LoadImageCommon(const std::string& imageName, ISearchPath& searchPath, IwiVersion expectedIwiVersion, CommonImageLoaderHashType hashType);
} // namespace image
