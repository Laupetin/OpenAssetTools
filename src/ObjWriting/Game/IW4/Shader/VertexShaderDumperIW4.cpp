#include "VertexShaderDumperIW4.h"

#include "Shader/ShaderCommon.h"

using namespace IW4;
using namespace ::shader;

namespace IW4::shader
{
    bool VertexShaderDumper::ShouldDump(XAssetInfo<MaterialVertexShader>* asset)
    {
        return true;
    }

    void VertexShaderDumper::DumpAsset(AssetDumpingContext& context, XAssetInfo<MaterialVertexShader>* asset)
    {
        const auto* vertexShader = asset->Asset();
        const auto shaderFile = context.OpenAssetFile(GetFileNameForVertexShaderAssetName(asset->m_name));

        if (!shaderFile)
            return;

        shaderFile->write(reinterpret_cast<const char*>(vertexShader->prog.loadDef.program),
                          static_cast<std::streamsize>(vertexShader->prog.loadDef.programSize) * 4u);
    }
} // namespace IW4::shader
