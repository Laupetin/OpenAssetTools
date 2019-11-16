#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T6/T6.h"

class AssetDumperQdb final : public AbstractAssetDumper<T6::Qdb>
{
protected:
    std::string GetFileNameForAsset(T6::Qdb* asset) override;
    void DumpAsset(T6::Qdb* asset, FileAPI::File* out) override;
};