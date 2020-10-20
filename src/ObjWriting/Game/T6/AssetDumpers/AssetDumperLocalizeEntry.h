#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T6/T6.h"

namespace T6
{
    class AssetDumperLocalizeEntry final : public IAssetDumper<LocalizeEntry>
    {
    public:
        void DumpPool(Zone* zone, AssetPool<LocalizeEntry>* pool, const std::string& basePath) override;
    };
}
