#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T6/T6.h"

namespace T6::techset
{
    class Dumper final : public AbstractAssetDumper<MaterialTechniqueSet>
    {
    protected:
        bool ShouldDump(XAssetInfo<MaterialTechniqueSet>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<MaterialTechniqueSet>* asset) override;
    };
} // namespace T6::techset
