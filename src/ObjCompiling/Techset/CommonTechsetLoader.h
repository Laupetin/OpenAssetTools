#pragma once

#include "Asset/AssetCreationContext.h"
#include "Techset/CommonTechset.h"

#include <memory>

namespace techset
{
    std::unique_ptr<CommonTechset>
        LoadCommonTechset(const std::string& assetName, const CommonTechniqueTypeNames& techniqueTypeNames, ISearchPath& searchPath, bool& failure);
} // namespace techset
