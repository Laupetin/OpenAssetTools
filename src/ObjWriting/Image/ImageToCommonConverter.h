#pragma once

#include "Image/Texture.h"
#include "Pool/XAssetInfo.h"
#include "SearchPath/ISearchPath.h"

#include <memory>

namespace image
{
    class ToCommonConverter
    {
    public:
        ToCommonConverter() = default;
        virtual ~ToCommonConverter() = default;
        ToCommonConverter(const ToCommonConverter& other) = default;
        ToCommonConverter(ToCommonConverter&& other) noexcept = default;
        ToCommonConverter& operator=(const ToCommonConverter& other) = default;
        ToCommonConverter& operator=(ToCommonConverter&& other) noexcept = default;

        static ToCommonConverter* GetForGame(GameId gameId);

        virtual std::unique_ptr<Texture> Convert(const XAssetInfoGeneric& assetInfo, ISearchPath& searchPath) = 0;
    };
} // namespace image
