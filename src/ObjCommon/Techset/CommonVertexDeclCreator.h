#pragma once

#include "CommonTechnique.h"

#include <optional>
#include <string>

namespace techset
{
    std::optional<CommonVertexDeclaration> CreateVertexDeclFromName(const std::string& name, const CommonStreamRoutingInfos& routingInfos);
}
