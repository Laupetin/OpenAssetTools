#include "ZoneDefWriterIW4.h"
#include "Game/IW4/GameIW4.h"
#include "Game/IW4/CommonIW4.h"
#include "Game/IW4/GameAssetPoolIW4.h"

#include <sstream>
#include <iomanip>
#include <cassert>

using namespace IW4;

namespace IW4
{
    class ZoneDefWriterInternal final : public AbstractZoneDefWriter
    {
        void WriteContent() const
        {
            const auto* pools = dynamic_cast<GameAssetPoolIW4*>(m_zone->GetPools());

            assert(pools);
            if (!pools)
                return;

            // Localized strings are all collected in one string file. So only add this to the zone file.
            if (!pools->m_localize->m_asset_lookup.empty())
            {
                WriteEntry(pools->GetAssetTypeName(ASSET_TYPE_LOCALIZE_ENTRY), m_zone->m_name);
            }

            for (const auto& asset : *pools)
            {
                switch (asset->m_type)
                {
                case ASSET_TYPE_LOCALIZE_ENTRY:
                    break;

                default:
                    WriteEntry(pools->GetAssetTypeName(asset->m_type), asset->m_name);
                    break;
                }
            }
        }

    public:
        ZoneDefWriterInternal(Zone* zone, FileAPI::IFile* file)
            : AbstractZoneDefWriter(zone, file)
        {
        }

        void WriteZoneDef() override
        {
            WriteComment("Call Of Duty: Modern Warfare 2");
            WriteMetaData(META_DATA_KEY_GAME, "iw4");
            EmptyLine();

            WriteContent();
        }
    };
}

bool ZoneDefWriter::CanHandleZone(Zone* zone) const
{
    return zone->m_game == &g_GameIW4;
}

void ZoneDefWriter::WriteZoneDef(Zone* zone, FileAPI::IFile* file) const
{
    ZoneDefWriterInternal writer(zone, file);
    writer.WriteZoneDef();
}
