#include "AssetDumperXModel.h"

#include "Game/IW4/XModel/XModelDumperIW4.h"

using namespace IW4;

bool AssetDumperXModel::ShouldDump(XAssetInfo<XModel>* asset)
{
    return !asset->m_name.empty() && asset->m_name[0] != ',';
}

void AssetDumperXModel::DumpAsset(AssetDumpingContext& context, XAssetInfo<XModel>* asset)
{
    DumpXModel(context, asset);
}
