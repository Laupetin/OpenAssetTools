#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T6/T6.h"

class AssetDumperLocalizeEntry final : public IAssetDumper<T6::LocalizeEntry>
{
public:
    void DumpPool(AssetPool<T6::LocalizeEntry>* pool, const std::string& basePath) override;
};