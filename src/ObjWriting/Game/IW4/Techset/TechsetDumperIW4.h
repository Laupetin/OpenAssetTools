#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW4/IW4.h"

namespace IW4::techset
{
    class Dumper final : public AbstractAssetDumper<MaterialTechniqueSet>
    {
    protected:
        bool ShouldDump(XAssetInfo<MaterialTechniqueSet>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<MaterialTechniqueSet>* asset) override;
    };
} // namespace IW4::techset
