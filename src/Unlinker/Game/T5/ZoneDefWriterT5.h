#pragma once

#include "ContentLister/ZoneDefWriter.h"

namespace T5
{
    class ZoneDefWriter final : public AbstractZoneDefWriter
    {
    protected:
        void WriteMetaData(ZoneDefinitionOutputStream& stream, const UnlinkerArgs* args, Zone* zone) const override;
        void WriteContent(ZoneDefinitionOutputStream& stream, const UnlinkerArgs* args, Zone* zone) const override;

    public:
        bool CanHandleZone(Zone* zone) const override;
    };
} // namespace T5
