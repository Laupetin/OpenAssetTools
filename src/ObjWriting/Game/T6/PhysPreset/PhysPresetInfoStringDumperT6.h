#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T6/T6.h"

namespace phys_preset
{
    class InfoStringDumperT6 final : public AbstractAssetDumper<T6::AssetPhysPreset>
    {
    public:
        explicit InfoStringDumperT6(const AssetPool<T6::AssetPhysPreset::Type>& pool);

    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<T6::AssetPhysPreset::Type>& asset) override;
    };
} // namespace phys_preset
