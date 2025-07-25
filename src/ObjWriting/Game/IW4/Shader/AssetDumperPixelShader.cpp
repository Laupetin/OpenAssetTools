#include "AssetDumperPixelShader.h"

#include <sstream>

using namespace IW4;

bool AssetDumperPixelShader::ShouldDump(XAssetInfo<MaterialPixelShader>* asset)
{
    return true;
}

void AssetDumperPixelShader::DumpAsset(AssetDumpingContext& context, XAssetInfo<MaterialPixelShader>* asset)
{
    const auto* pixelShader = asset->Asset();

    std::ostringstream ss;
    ss << "shader_bin/ps_" << pixelShader->name << ".cso";

    const auto shaderFile = context.OpenAssetFile(ss.str());

    if (!shaderFile)
        return;

    shaderFile->write(reinterpret_cast<const char*>(pixelShader->prog.loadDef.program),
                      static_cast<std::streamsize>(pixelShader->prog.loadDef.programSize) * 4u);
}
