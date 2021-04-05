#pragma once

#include <memory>
#include <ostream>

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T6/T6.h"

namespace T6
{
    class AssetDumperSndBank final : public IAssetDumper<SndBank>
    {
        std::unique_ptr<std::ostream> OpenOutputFile(AssetDumpingContext& context, const std::string& outputFileName) const;
        void DumpSndBank(AssetDumpingContext& context, const XAssetInfo<SndBank>* sndBankInfo);

    public:
        void DumpPool(AssetDumpingContext& context, AssetPool<SndBank>* pool) override;
    };
}
