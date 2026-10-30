#include "XModelToCommonConverterCODO.h"

#include "Utils/Logging/Log.h"

namespace xmodel
{
    std::optional<XModelCommon> ToCommonConverterCODO::Convert(const XAssetInfoGeneric& assetInfo, const unsigned lod)
    {
        con::error("Dumping xmodels is not supported for CODO yet");
        return std::nullopt;
    }
} // namespace xmodel
