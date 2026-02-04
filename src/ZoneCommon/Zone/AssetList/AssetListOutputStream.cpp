#include "AssetListOutputStream.h"

AssetListOutputStream::AssetListOutputStream(std::ostream& stream, const GameId game)
    : m_stream(stream),
      m_game(IGame::GetGameById(game))
{
}

void AssetListOutputStream::WriteEntry(const AssetListEntry& entry)
{
    m_stream.WriteColumn(*m_game->GetAssetTypeName(entry.m_type));
    m_stream.WriteColumn(entry.m_name);
    m_stream.NextRow();
}
