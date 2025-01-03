#pragma once

#include "AssetList.h"
#include "Game/IGame.h"
#include "SearchPath/ISearchPath.h"

#include <optional>
#include <string>

class AssetListReader
{
public:
    AssetListReader(ISearchPath& searchPath, GameId game);

    std::optional<AssetList> ReadAssetList(const std::string& zoneName, bool logMissing = true) const;

private:
    ISearchPath& m_search_path;
    GameId m_game;
};
