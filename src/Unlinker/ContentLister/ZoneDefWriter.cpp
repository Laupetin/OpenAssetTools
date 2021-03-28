#include "ZoneDefWriter.h"

void AbstractZoneDefWriter::WriteZoneDef(std::ostream& stream, const UnlinkerArgs* args, Zone* zone) const
{
    ZoneDefinitionOutputStream out(stream);

    out.WriteComment(zone->m_game->GetFullName());
    out.WriteMetaData(META_DATA_KEY_GAME, zone->m_game->GetShortName());
    out.EmptyLine();

    if(args->m_use_gdt)
    {
        out.WriteComment("Load asset gdt files");
        out.WriteMetaData(META_DATA_KEY_GDT, zone->m_name);
        out.EmptyLine();
    }

    WriteMetaData(out, args, zone);
    WriteContent(out, args, zone);
}