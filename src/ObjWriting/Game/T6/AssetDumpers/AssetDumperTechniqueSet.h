#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T6/T6.h"

class AssetDumperTechniqueSet final : public AbstractAssetDumper<T6::AssetTechniqueSet>
{
protected:
    void DumpAsset(AssetDumpingContext& context, const XAssetInfo<T6::AssetTechniqueSet::Type>& asset) override;
};
