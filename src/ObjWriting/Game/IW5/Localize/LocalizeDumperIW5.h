#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW5/IW5.h"

namespace localize
{
    class DumperIW5 final : public AbstractSingleProgressAssetDumper<IW5::AssetLocalize>
    {
    public:
        explicit DumperIW5(const AssetPool<IW5::AssetLocalize::Type>& pool);

        void Dump(AssetDumpingContext& context) override;
    };
} // namespace localize
