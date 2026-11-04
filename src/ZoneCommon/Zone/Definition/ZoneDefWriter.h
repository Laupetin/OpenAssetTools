#pragma once

#include "Pool/XAssetInfo.h"
#include "Zone/Definition/ZoneDefinitionStream.h"
#include "Zone/Zone.h"

#include <string>
#include <unordered_set>
#include <vector>

class ZoneDefFilter
{
public:
    static ZoneDefFilter AllEntries();
    static ZoneDefFilter WithOnlyRootAssets(const Zone& zone);

    void RestrictToAssetTypes(const Zone& zone, std::vector<bool> assetTypes);

    [[nodiscard]] bool ShouldWriteAssetType(asset_type_t assetType) const;
    [[nodiscard]] bool ShouldWriteAsset(const XAssetInfoGeneric& asset) const;
    [[nodiscard]] std::string EntryName(const XAssetInfoGeneric& asset) const;

private:
    ZoneDefFilter() = default;

    std::unordered_set<const XAssetInfoGeneric*> m_non_root_asset_lookup;
    std::vector<bool> m_asset_types;
    std::unordered_set<const XAssetInfoGeneric*> m_reference_asset_lookup;
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

    virtual void WriteZoneDef(std::ostream& stream, const Zone& zone, bool useGdt, bool minimalZone, std::vector<bool> assetTypes) const = 0;

    static const IZoneDefWriter* GetZoneDefWriterForGame(GameId game);
};

class AbstractZoneDefWriter : public IZoneDefWriter
{
public:
    void WriteZoneDef(std::ostream& stream, const Zone& zone, bool useGdt, bool minimalZone, std::vector<bool> assetTypes) const override;

protected:
    virtual void WriteMetaData(ZoneDefinitionOutputStream& stream, const Zone& zone) const = 0;
    virtual void WriteContent(ZoneDefinitionOutputStream& stream, const Zone& zone, const ZoneDefFilter& filter) const = 0;
};
