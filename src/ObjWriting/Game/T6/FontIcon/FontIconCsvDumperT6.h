#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T6/T6.h"

namespace font_icon
{
    class CsvDumperT6 final : public AbstractAssetDumper<T6::FontIcon>
    {
    protected:
        bool ShouldDump(XAssetInfo<T6::FontIcon>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<T6::FontIcon>* asset) override;
    };
} // namespace font_icon
