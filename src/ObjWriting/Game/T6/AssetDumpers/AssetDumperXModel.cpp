#include "AssetDumperXModel.h"

#include "Game/T6/XModel/XModelDumperT6.h"

using namespace T6;

bool AssetDumperXModel::ShouldDump(XAssetInfo<XModel>* asset)
{
    return !asset->m_name.empty() && asset->m_name[0] != ',';
}

void AssetDumperXModel::DumpAsset(AssetDumpingContext& context, XAssetInfo<XModel>* asset)
{
    DumpXModel(context, asset);
}
