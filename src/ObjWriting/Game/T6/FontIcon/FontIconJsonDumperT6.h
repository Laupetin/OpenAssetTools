#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T6/T6.h"

namespace font_icon
{
    class JsonDumperT6 final : public AbstractAssetDumper<T6::AssetFontIcon>
    {
    public:
        explicit JsonDumperT6(const AssetPool<T6::AssetFontIcon::Type>& pool);

    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<T6::AssetFontIcon::Type>& asset) override;
    };
} // namespace font_icon
