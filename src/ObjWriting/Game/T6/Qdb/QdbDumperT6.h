#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T6/T6.h"

namespace T6::qdb
{
    class Dumper final : public AbstractAssetDumper<Qdb>
    {
    protected:
        bool ShouldDump(XAssetInfo<Qdb>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<Qdb>* asset) override;
    };
} // namespace T6::qdb
