#pragma once

#include "Zone/Definition/ZoneDefinitionStream.h"
#include "Zone/Zone.h"

#include <unordered_set>

class XAssetInfoGeneric;

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
protected:
    static constexpr auto META_DATA_KEY_GAME = "game";
    static constexpr auto META_DATA_KEY_GDT = "gdt";

    using DependencyAssetLookup = std::unordered_set<const XAssetInfoGeneric*>;

    static DependencyAssetLookup CreateDependencyAssetLookup(const Zone& zone, bool minimalZone);
    static bool ShouldWriteAsset(const XAssetInfoGeneric& asset, const DependencyAssetLookup& dependencyAssets);

    virtual void WriteMetaData(ZoneDefinitionOutputStream& stream, const Zone& zone) const = 0;
    virtual void WriteContent(ZoneDefinitionOutputStream& stream, const Zone& zone, const DependencyAssetLookup& dependencyAssets) const = 0;

public:
    void WriteZoneDef(std::ostream& stream, const Zone& zone, bool useGdt, bool minimalZone) const override;
};
