#pragma once

#include "Asset/AssetCreationContext.h"
#include "Techset/CommonTechnique.h"

#include <memory>

namespace techset
{
    std::unique_ptr<CommonTechnique> LoadCommonTechnique(const std::string& techniqueName,
                                                         const CommonCodeSourceInfos& codeSourceInfos,
                                                         const CommonStreamRoutingInfos& routingInfos,
                                                         ISearchPath& searchPath);
} // namespace techset
