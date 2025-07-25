#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T6/T6.h"

namespace T6
{
    class AssetDumperScriptParseTree final : public AbstractAssetDumper<ScriptParseTree>
    {
    protected:
        bool ShouldDump(XAssetInfo<ScriptParseTree>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<ScriptParseTree>* asset) override;
    };
} // namespace T6
