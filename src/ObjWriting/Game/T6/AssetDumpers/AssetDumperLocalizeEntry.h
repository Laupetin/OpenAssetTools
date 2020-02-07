#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T6/T6.h"
#include "Game/GameLanguage.h"

class AssetDumperLocalizeEntry final : public IAssetDumper<T6::LocalizeEntry>
{
    static std::string GetNameOfLanguage(GameLanguage language);

public:
    void DumpPool(Zone* zone, AssetPool<T6::LocalizeEntry>* pool, const std::string& basePath) override;
};