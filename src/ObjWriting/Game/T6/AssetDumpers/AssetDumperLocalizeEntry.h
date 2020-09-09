#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T6/T6.h"

namespace T6
{
    class AssetDumperLocalizeEntry final : public IAssetDumper<T6::LocalizeEntry>
    {
    public:
        void DumpPool(Zone* zone, AssetPool<T6::LocalizeEntry>* pool, const std::string& basePath) override;
    };
}
