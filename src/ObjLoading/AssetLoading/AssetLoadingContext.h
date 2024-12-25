#pragma once

#include "Gdt/IGdtQueryable.h"
#include "IZoneAssetLoaderState.h"
#include "Obj/Gdt/Gdt.h"
#include "SearchPath/ISearchPath.h"
#include "Zone/Zone.h"

#include <type_traits>
#include <typeindex>
#include <unordered_map>

class AssetLoadingContext final : public IGdtQueryable
{
public:
    AssetLoadingContext(Zone& zone, ISearchPath& rawSearchPath, std::vector<Gdt*> gdtFiles);
    GdtEntry* GetGdtEntryByGdfAndName(const std::string& gdfName, const std::string& entryName) override;

    template<typename T> T* GetZoneAssetLoaderState()
    {
        static_assert(std::is_base_of_v<IZoneAssetLoaderState, T>, "T must inherit IZoneAssetLoaderState");
        // T must also have a public default constructor

        const auto foundEntry = m_zone_asset_loader_states.find(typeid(T));
        if (foundEntry != m_zone_asset_loader_states.end())
            return dynamic_cast<T*>(foundEntry->second.get());

        auto newState = std::make_unique<T>();
        newState->SetZone(&m_zone);
        auto* newStatePtr = newState.get();
        m_zone_asset_loader_states.emplace(std::make_pair<std::type_index, std::unique_ptr<IZoneAssetLoaderState>>(typeid(T), std::move(newState)));
        return newStatePtr;
    }

public:
    Zone& m_zone;
    ISearchPath& m_raw_search_path;
    std::unordered_map<std::string, asset_type_t> m_ignored_asset_map;

    std::unordered_map<std::type_index, std::unique_ptr<IZoneAssetLoaderState>> m_zone_asset_loader_states;
};
