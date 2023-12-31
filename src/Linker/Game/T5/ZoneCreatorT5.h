#pragma once
#include "Zone/ZoneTypes.h"
#include "ZoneCreation/IZoneCreator.h"

#include <string>
#include <unordered_map>

namespace T5
{
    class ZoneCreator final : public IZoneCreator
    {
        std::unordered_map<std::string, asset_type_t> m_asset_types_by_name;

        void AddAssetTypeName(asset_type_t assetType, std::string name);
        static std::vector<Gdt*> CreateGdtList(ZoneCreationContext& context);
        bool CreateIgnoredAssetMap(const ZoneCreationContext& context, std::unordered_map<std::string, asset_type_t>& ignoredAssetMap) const;
        void CreateZoneAssetPools(Zone* zone) const;

    public:
        ZoneCreator();

        _NODISCARD bool SupportsGame(const std::string& gameName) const override;
        _NODISCARD std::unique_ptr<Zone> CreateZoneForDefinition(ZoneCreationContext& context) const override;
    };
} // namespace T5
