#pragma once

#include "Pool/XAssetInfo.h"
#include "XModel/XModelCommon.h"

#include <optional>

namespace xmodel
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

        virtual std::optional<XModelCommon> Convert(const XAssetInfoGeneric& assetInfo, unsigned lod) = 0;
    };
} // namespace xmodel
