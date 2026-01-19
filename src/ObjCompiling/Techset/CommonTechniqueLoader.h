#pragma once

#include "Asset/AssetCreationContext.h"
#include "Techset/CommonTechnique.h"

#include <memory>

namespace techset
{
    std::unique_ptr<CommonTechnique>
        LoadCommonTechnique(const AssetCreationContext& context, const CommonCodeSourceInfos& codeSourceInfos, const CommonStreamRoutingInfos& routingInfos);
} // namespace techset
