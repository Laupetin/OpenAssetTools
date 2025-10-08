#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW5/IW5.h"

namespace script
{
    class DumperIW5 final : public AbstractAssetDumper<IW5::ScriptFile>
    {
    protected:
        bool ShouldDump(XAssetInfo<IW5::ScriptFile>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<IW5::ScriptFile>* asset) override;
    };
} // namespace script
