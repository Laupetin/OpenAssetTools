#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T6/T6.h"

class AssetDumperScriptParseTree final : public AbstractAssetDumper<T6::ScriptParseTree>
{
protected:
    std::string GetFileNameForAsset(T6::ScriptParseTree* asset) override;
    void DumpAsset(T6::ScriptParseTree* asset, FileAPI::File* out) override;
};