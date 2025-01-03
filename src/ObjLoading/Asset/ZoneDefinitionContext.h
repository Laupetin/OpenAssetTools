#pragma once

#include "Zone/Definition/ZoneDefinition.h"

class ZoneDefinitionContext
{
public:
    ZoneDefinitionContext(const ZoneDefinition& zoneDefinition)
        : m_zone_definition(zoneDefinition),
          m_asset_index_in_definition(0u)
    {
    }

    const ZoneDefinition& m_zone_definition;
    unsigned m_asset_index_in_definition;
};
