#include "AttachmentUniqueCommon.h"

#include <format>

namespace attachment_unique
{
    std::string GetFileNameForAssetName(const std::string& assetName)
    {
        return std::format("attachmentunique/{}", assetName);
    }
} // namespace attachment_unique
