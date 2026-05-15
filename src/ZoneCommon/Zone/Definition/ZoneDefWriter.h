#pragma once

#include "Pool/XAssetInfo.h"
#include "Zone/Definition/ZoneDefinitionStream.h"
#include "Zone/Zone.h"

#include <unordered_set>

class ZoneDefFilter
{
public:
    static ZoneDefFilter AllEntries();
    static ZoneDefFilter WithOnlyRootAssets(const Zone& zone);

    [[nodiscard]] bool ShouldWriteAsset(const XAssetInfoGeneric& asset) const;

private:
    ZoneDefFilter() = default;

    std::unordered_set<const XAssetInfoGeneric*> m_non_root_asset_lookup;
};

class IZoneDefWriter
{
public:
    IZoneDefWriter() = default;
    virtual ~IZoneDefWriter() = default;
    IZoneDefWriter(const IZoneDefWriter& other) = default;
    IZoneDefWriter(IZoneDefWriter&& other) noexcept = default;
    IZoneDefWriter& operator=(const IZoneDefWriter& other) = default;
    IZoneDefWriter& operator=(IZoneDefWriter&& other) noexcept = default;

    virtual void WriteZoneDef(std::ostream& stream, const Zone& zone, bool useGdt, bool minimalZone) const = 0;

    static const IZoneDefWriter* GetZoneDefWriterForGame(GameId game);
};

class AbstractZoneDefWriter : public IZoneDefWriter
{
public:
    void WriteZoneDef(std::ostream& stream, const Zone& zone, bool useGdt, bool minimalZone) const override;

protected:
    virtual void WriteMetaData(ZoneDefinitionOutputStream& stream, const Zone& zone) const = 0;
    virtual void WriteContent(ZoneDefinitionOutputStream& stream, const Zone& zone, const ZoneDefFilter& filter) const = 0;
};
