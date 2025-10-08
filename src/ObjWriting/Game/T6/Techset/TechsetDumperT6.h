#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T6/T6.h"

namespace techset
{
    class DumperT6 final : public AbstractAssetDumper<T6::MaterialTechniqueSet>
    {
    protected:
        bool ShouldDump(XAssetInfo<T6::MaterialTechniqueSet>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<T6::MaterialTechniqueSet>* asset) override;
    };
} // namespace techset
