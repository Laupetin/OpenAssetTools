#include "AssetDumperPixelShader.h"

using namespace IW4;

bool AssetDumperPixelShader::ShouldDump(XAssetInfo<MaterialPixelShader>* asset)
{
    return true;
}

void AssetDumperPixelShader::DumpAsset(AssetDumpingContext& context, XAssetInfo<MaterialPixelShader>* asset)
{
}
