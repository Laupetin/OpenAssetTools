#include "ScriptDumperT6.h"

using namespace T6;

namespace script
{
    DumperT6::DumperT6(const AssetPool<AssetScript::Type>& pool)
        : AbstractAssetDumper(pool)
    {
    }

    void DumperT6::DumpAsset(AssetDumpingContext& context, const XAssetInfo<AssetScript::Type>& asset)
    {
        const auto* scriptParseTree = asset.Asset();
        const auto assetFile = context.OpenAssetFile(asset.m_name);

        if (!assetFile)
            return;

        auto& stream = *assetFile;
        stream.write(scriptParseTree->buffer, scriptParseTree->len);
    }
} // namespace script
