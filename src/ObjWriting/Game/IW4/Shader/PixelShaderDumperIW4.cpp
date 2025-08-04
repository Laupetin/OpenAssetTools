#include "PixelShaderDumperIW4.h"

#include "Shader/ShaderCommon.h"

using namespace IW4;
using namespace ::shader;

namespace IW4::shader
{
    bool PixelShaderDumper::ShouldDump(XAssetInfo<MaterialPixelShader>* asset)
    {
        return true;
    }

    void PixelShaderDumper::DumpAsset(AssetDumpingContext& context, XAssetInfo<MaterialPixelShader>* asset)
    {
        const auto* pixelShader = asset->Asset();
        const auto shaderFile = context.OpenAssetFile(GetFileNameForPixelShaderAssetName(asset->m_name));

        if (!shaderFile)
            return;

        shaderFile->write(reinterpret_cast<const char*>(pixelShader->prog.loadDef.program),
                          static_cast<std::streamsize>(pixelShader->prog.loadDef.programSize) * 4u);
    }
} // namespace IW4::shader
