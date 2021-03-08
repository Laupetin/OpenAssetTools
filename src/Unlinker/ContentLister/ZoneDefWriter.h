#pragma once

#include "Zone/Zone.h"
#include "Zone/Definition/ZoneDefinitionStream.h"

class IZoneDefWriter
{
public:
    IZoneDefWriter() = default;
    virtual ~IZoneDefWriter() = default;
    IZoneDefWriter(const IZoneDefWriter& other) = default;
    IZoneDefWriter(IZoneDefWriter&& other) noexcept = default;
    IZoneDefWriter& operator=(const IZoneDefWriter& other) = default;
    IZoneDefWriter& operator=(IZoneDefWriter&& other) noexcept = default;

    virtual bool CanHandleZone(Zone* zone) const = 0;
    virtual void WriteZoneDef(std::ostream& stream, Zone* zone) const = 0;
};

class AbstractZoneDefWriter : public IZoneDefWriter
{
protected:
    static constexpr const char* META_DATA_KEY_GAME = "game";
    
    virtual void WriteMetaData(ZoneDefinitionOutputStream& stream, Zone* zone) const = 0;
    virtual void WriteContent(ZoneDefinitionOutputStream& stream, Zone* zone) const = 0;

public:
    void WriteZoneDef(std::ostream& stream, Zone* zone) const override;
};