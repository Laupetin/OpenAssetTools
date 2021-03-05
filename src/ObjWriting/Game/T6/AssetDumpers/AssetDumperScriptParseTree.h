#pragma once

#include "Dumping/AbstractFileDumper.h"
#include "Game/T6/T6.h"

namespace T6
{
    class AssetDumperScriptParseTree final : public AbstractFileDumper<ScriptParseTree>
    {
    protected:
        bool ShouldDump(XAssetInfo<ScriptParseTree>* asset) override;
        std::string GetFileNameForAsset(Zone* zone, XAssetInfo<ScriptParseTree>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<ScriptParseTree>* asset, std::ostream& stream) override;
    };
}
