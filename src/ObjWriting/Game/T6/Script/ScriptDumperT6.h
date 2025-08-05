#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T6/T6.h"

namespace script
{
    class DumperT6 final : public AbstractAssetDumper<T6::ScriptParseTree>
    {
    protected:
        bool ShouldDump(XAssetInfo<T6::ScriptParseTree>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<T6::ScriptParseTree>* asset) override;
    };
} // namespace script
