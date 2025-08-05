#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T6/T6.h"

namespace qdb
{
    class DumperT6 final : public AbstractAssetDumper<T6::Qdb>
    {
    protected:
        bool ShouldDump(XAssetInfo<T6::Qdb>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<T6::Qdb>* asset) override;
    };
} // namespace qdb
