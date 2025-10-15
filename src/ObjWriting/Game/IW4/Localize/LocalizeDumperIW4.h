#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW4/IW4.h"

namespace localize
{
    class DumperIW4 final : public AbstractSingleProgressAssetDumper<IW4::AssetLocalize>
    {
    public:
        explicit DumperIW4(const AssetPool<IW4::AssetLocalize::Type>& pool);

        void Dump(AssetDumpingContext& context) override;
    };
} // namespace localize
