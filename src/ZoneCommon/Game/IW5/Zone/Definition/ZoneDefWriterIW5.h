#pragma once

#include "Zone/Definition/ZoneDefWriter.h"

namespace IW5
{
    class ZoneDefWriter final : public AbstractZoneDefWriter
    {
    protected:
        void WriteMetaData(ZoneDefinitionOutputStream& stream, const Zone& zone) const override;
        void WriteContent(ZoneDefinitionOutputStream& stream, const Zone& zone) const override;
    };
} // namespace IW5
