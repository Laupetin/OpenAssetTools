#include "ZoneDefWriter.h"

void AbstractZoneDefWriter::WriteZoneDef(std::ostream& stream, Zone* zone) const
{
    ZoneDefinitionOutputStream out(stream);

    out.WriteComment(zone->m_game->GetFullName());
    out.WriteMetaData(META_DATA_KEY_GAME, zone->m_game->GetShortName());
    out.EmptyLine();

    WriteMetaData(out, zone);
    WriteContent(out, zone);
}