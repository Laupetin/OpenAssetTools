#pragma once

#include "Asset/AssetCreationContext.h"
#include "CommonShaderArgCreator.h"
#include "Techset/CommonTechnique.h"

#include <memory>
#include <string>

namespace techset
{
    std::unique_ptr<CommonTechnique> LoadCommonTechnique(const std::string& techniqueName,
                                                         const CommonCodeSourceInfos& codeSourceInfos,
                                                         const CommonStreamRoutingInfos& routingInfos,
                                                         CommonShaderArgCreator& commonShaderArgCreator,
                                                         ISearchPath& searchPath,
                                                         bool& failure);
} // namespace techset
