#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T6/T6.h"

class AssetDumperQdb final : public AbstractAssetDumper<T6::Qdb>
{
protected:
    bool ShouldDump(T6::Qdb* asset) override;
    std::string GetFileNameForAsset(Zone* zone, T6::Qdb* asset) override;
    void DumpAsset(Zone* zone, T6::Qdb* asset, FileAPI::File* out) override;
};