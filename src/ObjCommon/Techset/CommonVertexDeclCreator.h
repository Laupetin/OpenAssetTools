#pragma once

#include "CommonTechnique.h"

#include <optional>
#include <string>

namespace techset
{
    std::optional<bool> HasOptionalSourceByName(const std::string& name, const CommonStreamRoutingInfos& routingInfos);
    std::optional<CommonVertexDeclaration> CreateVertexDeclFromName(const std::string& name, const CommonStreamRoutingInfos& routingInfos);
} // namespace techset
