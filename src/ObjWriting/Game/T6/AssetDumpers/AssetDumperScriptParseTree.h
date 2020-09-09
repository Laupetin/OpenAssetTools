#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T6/T6.h"

namespace T6
{
    class AssetDumperScriptParseTree final : public AbstractAssetDumper<T6::ScriptParseTree>
    {
    protected:
        bool ShouldDump(T6::ScriptParseTree* asset) override;
        std::string GetFileNameForAsset(Zone* zone, T6::ScriptParseTree* asset) override;
        void DumpAsset(Zone* zone, T6::ScriptParseTree* asset, FileAPI::File* out) override;
    };
}
