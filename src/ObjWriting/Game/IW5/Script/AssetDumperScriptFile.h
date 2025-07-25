#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW5/IW5.h"

namespace IW5
{
    class AssetDumperScriptFile final : public AbstractAssetDumper<ScriptFile>
    {
    protected:
        bool ShouldDump(XAssetInfo<ScriptFile>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<ScriptFile>* asset) override;
    };
} // namespace IW5
