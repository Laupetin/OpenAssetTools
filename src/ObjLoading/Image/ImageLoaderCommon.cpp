#include "ImageLoaderCommon.h"

namespace image
{
    std::optional<AssetCreationResult> CommonImageLoaderResult::GetResultIfCancelled() const
    {
        if (m_failure)
            return AssetCreationResult::Failure();

        if (!m_texture)
            return AssetCreationResult::NoAction();

        return std::nullopt;
    }
} // namespace image
