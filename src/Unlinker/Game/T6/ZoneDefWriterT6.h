#pragma once

#include "ContentLister/ZoneDefWriter.h"
#include "Game/T6/T6.h"

namespace T6
{
    class ZoneDefWriter final : public AbstractZoneDefWriter
    {
        static void WriteKeyValuePair(ZoneDefinitionOutputStream& stream, KeyValuePair* kvp);

    protected:
        void WriteMetaData(ZoneDefinitionOutputStream& stream, const UnlinkerArgs* args, Zone* zone) const override;
        void WriteContent(ZoneDefinitionOutputStream& stream, const UnlinkerArgs* args, Zone* zone) const override;

    public:
        bool CanHandleZone(Zone* zone) const override;
    };
} // namespace T6
