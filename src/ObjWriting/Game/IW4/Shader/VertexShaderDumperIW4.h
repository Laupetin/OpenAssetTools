#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW4/IW4.h"

namespace shader
{
    class VertexShaderDumperIW4 final : public AbstractAssetDumper<IW4::AssetVertexShader>
    {
    public:
        explicit VertexShaderDumperIW4(const AssetPool<IW4::AssetVertexShader::Type>& pool);

    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<IW4::AssetVertexShader::Type>& asset) override;
    };
} // namespace shader
