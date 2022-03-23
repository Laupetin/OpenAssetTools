#include "AssetDumperVertexDecl.h"

using namespace IW4;

bool AssetDumperVertexDecl::ShouldDump(XAssetInfo<MaterialVertexDeclaration>* asset)
{
    return true;
}

void AssetDumperVertexDecl::DumpAsset(AssetDumpingContext& context, XAssetInfo<MaterialVertexDeclaration>* asset)
{
}
