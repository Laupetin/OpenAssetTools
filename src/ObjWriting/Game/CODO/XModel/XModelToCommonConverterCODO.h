#pragma once

#include "Game/CODO/CODO.h"
#include "XModel/XModelToCommonConverter.h"

namespace xmodel
{
    class ToCommonConverterCODO final : public ToCommonConverter
    {
    public:
        std::optional<XModelCommon> Convert(const XAssetInfoGeneric& assetInfo, unsigned lod) override;
    };
} // namespace xmodel
