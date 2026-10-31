#pragma once

#include "Game/CODO/CODO.h"
#include "Image/ImageToCommonConverter.h"

namespace image
{
    class ToCommonConverterCODO final : public ToCommonConverter
    {
    public:
        std::unique_ptr<Texture> Convert(const XAssetInfoGeneric& assetInfo, ISearchPath& searchPath) override;
    };
} // namespace image
