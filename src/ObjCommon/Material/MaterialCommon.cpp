#include "MaterialCommon.h"

#include <algorithm>
#include <format>

namespace material
{
    std::string GetFileNameForAssetName(const std::string& assetName)
    {
        std::string sanitizedFileName(assetName);
        if (sanitizedFileName[0] == '*')
        {
            std::ranges::replace(sanitizedFileName, '*', '_');
            const auto parenthesisPos = sanitizedFileName.find('(');
            if (parenthesisPos != std::string::npos)
                sanitizedFileName.erase(parenthesisPos);
            sanitizedFileName = std::format("generated/{}", sanitizedFileName);
        }

        return std::format("materials/{}.json", sanitizedFileName);
    }
} // namespace material
