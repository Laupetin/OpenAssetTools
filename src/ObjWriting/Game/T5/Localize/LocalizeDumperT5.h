#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T5/T5.h"

namespace localize
{
    class DumperT5 final : public AbstractSingleProgressAssetDumper<T5::AssetLocalize>
    {
    public:
        explicit DumperT5(const AssetPool<T5::AssetLocalize::Type>& pool);

        void Dump(AssetDumpingContext& context) override;
    };
} // namespace localize
