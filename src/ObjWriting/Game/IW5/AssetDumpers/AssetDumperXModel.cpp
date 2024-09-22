#include "AssetDumperXModel.h"

#include "Game/IW5/XModel/XModelDumperIW5.h"

using namespace IW5;

bool AssetDumperXModel::ShouldDump(XAssetInfo<XModel>* asset)
{
    return !asset->m_name.empty() && asset->m_name[0] != ',';
}

void AssetDumperXModel::DumpAsset(AssetDumpingContext& context, XAssetInfo<XModel>* asset)
{
    DumpXModel(context, asset);
}
