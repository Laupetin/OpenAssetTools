#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW4/IW4.h"
#include "InfoString/InfoString.h"

namespace phys_preset
{
    class InfoStringDumperIW4 final : public AbstractAssetDumper<IW4::PhysPreset>
    {
    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<IW4::PhysPreset>& asset) override;
    };
} // namespace phys_preset
