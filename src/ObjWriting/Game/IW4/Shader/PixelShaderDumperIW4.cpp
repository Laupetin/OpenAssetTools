#include "PixelShaderDumperIW4.h"

#include "Shader/ShaderCommon.h"

using namespace IW4;

namespace shader
{
    void PixelShaderDumperIW4::DumpAsset(AssetDumpingContext& context, const XAssetInfo<AssetPixelShader::Type>& asset)
    {
        const auto* pixelShader = asset.Asset();
        const auto shaderFile = context.OpenAssetFile(shader::GetFileNameForPixelShaderAssetName(asset.m_name));

        if (!shaderFile)
            return;

        shaderFile->write(reinterpret_cast<const char*>(pixelShader->prog.loadDef.program),
                          static_cast<std::streamsize>(pixelShader->prog.loadDef.programSize) * 4u);
    }
} // namespace shader
