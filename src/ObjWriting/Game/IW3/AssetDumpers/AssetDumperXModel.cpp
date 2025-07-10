#include "AssetDumperXModel.h"

#include "Game/IW3/XModel/XModelDumperIW3.h"

using namespace IW3;

bool AssetDumperXModel::ShouldDump(XAssetInfo<XModel>* asset)
{
    return !asset->m_name.empty() && asset->m_name[0] != ',';
}

void AssetDumperXModel::DumpAsset(AssetDumpingContext& context, XAssetInfo<XModel>* asset)
{
    DumpXModel(context, asset);
}
