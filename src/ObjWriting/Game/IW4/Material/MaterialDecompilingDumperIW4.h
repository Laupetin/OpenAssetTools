#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW4/IW4.h"

namespace material
{
    class DecompilingGdtDumperIW4 final : public AbstractAssetDumper<IW4::AssetMaterial>
    {
    public:
        explicit DecompilingGdtDumperIW4(const AssetPool<IW4::AssetMaterial::Type>& pool);

    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<IW4::AssetMaterial::Type>& asset) override;
    };
} // namespace material
