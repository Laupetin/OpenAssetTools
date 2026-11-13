#include "ImageToCommonConverterCODO.h"

#include "Utils/Logging/Log.h"

namespace image
{
    std::unique_ptr<Texture> ToCommonConverterCODO::Convert(const XAssetInfoGeneric& assetInfo, ISearchPath& searchPath)
    {
        con::error("Dumping images is not supported for CODO yet");
        return nullptr;
    }
} // namespace image
