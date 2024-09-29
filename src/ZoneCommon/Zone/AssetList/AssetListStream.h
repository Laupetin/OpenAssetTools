#pragma once
#include "AssetList.h"
#include "Csv/CsvStream.h"
#include "Game/IGame.h"

#include <Zone/AssetNameResolver.h>
#include <iostream>

class AssetListInputStream
{
public:
    AssetListInputStream(std::istream& stream, GameId game);

    bool NextEntry(AssetListEntry& entry, bool* failure) const;

private:
    CsvInputStream m_stream;
    const IAssetNameResolver* m_asset_name_resolver;
};

class AssetListOutputStream
{
public:
    AssetListOutputStream(std::ostream& stream, GameId game);

    void WriteEntry(const AssetListEntry& entry);

private:
    CsvOutputStream m_stream;
    const IAssetNameResolver* m_asset_name_resolver;
};
