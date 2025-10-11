#pragma once

#include "Zone/Definition/ZoneDefWriter.h"

namespace IW3
{
    class ZoneDefWriter final : public AbstractZoneDefWriter
    {
    protected:
        void WriteMetaData(ZoneDefinitionOutputStream& stream, const Zone& zone) const override;
        void WriteContent(ZoneDefinitionOutputStream& stream, const Zone& zone) const override;
    };
} // namespace IW3
