#pragma once

#include "ContentLister/ZoneDefWriter.h"

namespace IW3
{
    class ZoneDefWriter final : public AbstractZoneDefWriter
    {
    protected:
        void WriteMetaData(ZoneDefinitionOutputStream& stream, const UnlinkerArgs& args, const Zone& zone) const override;
        void WriteContent(ZoneDefinitionOutputStream& stream, const UnlinkerArgs& args, const Zone& zone) const override;
    };
} // namespace IW3
