#include "AssetDumperVertexShader.h"

using namespace IW4;

bool AssetDumperVertexShader::ShouldDump(XAssetInfo<MaterialVertexShader>* asset)
{
    return true;
}

void AssetDumperVertexShader::DumpAsset(AssetDumpingContext& context, XAssetInfo<MaterialVertexShader>* asset)
{
}
