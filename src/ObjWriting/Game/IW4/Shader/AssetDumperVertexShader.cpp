#include "AssetDumperVertexShader.h"

#include <sstream>

using namespace IW4;

bool AssetDumperVertexShader::ShouldDump(XAssetInfo<MaterialVertexShader>* asset)
{
    return true;
}

void AssetDumperVertexShader::DumpAsset(AssetDumpingContext& context, XAssetInfo<MaterialVertexShader>* asset)
{
    const auto* vertexShader = asset->Asset();

    std::ostringstream ss;
    ss << "shader_bin/vs_" << vertexShader->name << ".cso";

    const auto shaderFile = context.OpenAssetFile(ss.str());

    if (!shaderFile)
        return;

    shaderFile->write(reinterpret_cast<const char*>(vertexShader->prog.loadDef.program),
                      static_cast<std::streamsize>(vertexShader->prog.loadDef.programSize) * 4u);
}
