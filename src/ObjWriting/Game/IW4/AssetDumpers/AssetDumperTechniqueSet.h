#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW4/IW4.h"

namespace IW4
{
    class AssetDumperTechniqueSet final : public AbstractAssetDumper<MaterialTechniqueSet>
    {
        static std::string GetTechniqueFileName(const MaterialTechnique* technique);
        static std::string GetTechsetFileName(const MaterialTechniqueSet* techset);

    protected:
        bool ShouldDump(XAssetInfo<MaterialTechniqueSet>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<MaterialTechniqueSet>* asset) override;
    };
}
