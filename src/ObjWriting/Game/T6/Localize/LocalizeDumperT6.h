#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T6/T6.h"

namespace localize
{
    class DumperT6 final : public AbstractSingleProgressAssetDumper<T6::AssetLocalize>
    {
    public:
        explicit DumperT6(const AssetPool<T6::AssetLocalize::Type>& pool);

        void Dump(AssetDumpingContext& context) override;
    };
} // namespace localize
