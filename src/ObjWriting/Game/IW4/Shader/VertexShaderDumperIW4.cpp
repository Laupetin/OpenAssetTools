#include "VertexShaderDumperIW4.h"

#include "Shader/ShaderCommon.h"

using namespace IW4;

namespace shader
{
    void VertexShaderDumperIW4::DumpAsset(AssetDumpingContext& context, const XAssetInfo<AssetVertexShader::Type>& asset)
    {
        const auto* vertexShader = asset.Asset();
        const auto shaderFile = context.OpenAssetFile(shader::GetFileNameForVertexShaderAssetName(asset.m_name));

        if (!shaderFile)
            return;

        shaderFile->write(reinterpret_cast<const char*>(vertexShader->prog.loadDef.program),
                          static_cast<std::streamsize>(vertexShader->prog.loadDef.programSize) * 4u);
    }
} // namespace shader
