#pragma once

#include "AssetList.h"
#include "Csv/CsvStream.h"
#include "Game/IGame.h"

#include <iostream>

class AssetListOutputStream
{
public:
    AssetListOutputStream(std::ostream& stream, GameId game);

    void WriteEntry(const AssetListEntry& entry);

private:
    CsvOutputStream m_stream;
    const IGame* m_game;
};
