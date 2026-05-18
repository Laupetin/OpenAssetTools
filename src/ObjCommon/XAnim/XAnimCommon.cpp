#include "XAnimCommon.h"

#include <format>

namespace xanim
{
    std::string GetCompiledFileNameForAssetName(const std::string& assetName)
    {
        return std::format("xanim/{}", assetName);
    }
} // namespace xanim
