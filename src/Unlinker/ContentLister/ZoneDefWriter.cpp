#include "ZoneDefWriter.h"

#include "Game/IW3/ZoneDefWriterIW3.h"
#include "Game/IW4/ZoneDefWriterIW4.h"
#include "Game/IW5/ZoneDefWriterIW5.h"
#include "Game/T5/ZoneDefWriterT5.h"
#include "Game/T6/ZoneDefWriterT6.h"

#include <cassert>

const IZoneDefWriter* IZoneDefWriter::GetZoneDefWriterForGame(GameId game)
{
    static const IZoneDefWriter* zoneDefWriters[static_cast<unsigned>(GameId::COUNT)]{
        new IW3::ZoneDefWriter(),
        new IW4::ZoneDefWriter(),
        new IW5::ZoneDefWriter(),
        new T5::ZoneDefWriter(),
        new T6::ZoneDefWriter(),
    };

    assert(static_cast<unsigned>(game) < static_cast<unsigned>(GameId::COUNT));
    const auto* result = zoneDefWriters[static_cast<unsigned>(game)];
    assert(result);

    return result;
}

void AbstractZoneDefWriter::WriteZoneDef(std::ostream& stream, const UnlinkerArgs& args, const Zone& zone) const
{
    ZoneDefinitionOutputStream out(stream);
    const auto* game = IGame::GetGameById(zone.m_game_id);

    out.WriteComment(game->GetFullName());
    out.WriteMetaData(META_DATA_KEY_GAME, game->GetShortName());
    out.EmptyLine();

    if (args.m_use_gdt)
    {
        out.WriteComment("Load asset gdt files");
        out.WriteMetaData(META_DATA_KEY_GDT, zone.m_name);
        out.EmptyLine();
    }

    WriteMetaData(out, args, zone);
    WriteContent(out, args, zone);
}
