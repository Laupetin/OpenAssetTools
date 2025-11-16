#pragma once

#include "Dumping/AssetDumpingContext.h"
#include "Material/AbstractMaterialConstantZoneState.h"
#include "Techset/CommonTechnique.h"

namespace techset
{
    void DumpCommonTechnique(const AssetDumpingContext& context,
                             const CommonTechnique& technique,
                             const CommonCodeSourceInfos& codeSourceInfos,
                             const CommonStreamRoutingInfos& routingInfos,
                             const AbstractMaterialConstantZoneState& constantZoneState);
} // namespace techset
