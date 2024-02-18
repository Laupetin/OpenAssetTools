#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T6/T6.h"

namespace T6
{
    class AssetDumperTechniqueSet final : public AbstractAssetDumper<MaterialTechniqueSet>
    {
        static void DumpPixelShader(const AssetDumpingContext& context, const MaterialPixelShader* pixelShader);
        static void DumpVertexShader(const AssetDumpingContext& context, const MaterialVertexShader* vertexShader);

    protected:
        bool ShouldDump(XAssetInfo<MaterialTechniqueSet>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<MaterialTechniqueSet>* asset) override;
    };
} // namespace T6
