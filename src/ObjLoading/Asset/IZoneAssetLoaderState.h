#pragma once

#include "Zone/Zone.h"

#include <memory>
#include <typeindex>
#include <unordered_map>

class IZoneAssetLoaderState
{
protected:
    IZoneAssetLoaderState() = default;

public:
    virtual ~IZoneAssetLoaderState() = default;
    IZoneAssetLoaderState(const IZoneAssetLoaderState& other) = default;
    IZoneAssetLoaderState(IZoneAssetLoaderState&& other) noexcept = default;
    IZoneAssetLoaderState& operator=(const IZoneAssetLoaderState& other) = default;
    IZoneAssetLoaderState& operator=(IZoneAssetLoaderState&& other) noexcept = default;

    virtual void SetZone(Zone* zone)
    {
        // Do nothing by default
    }
};

class ZoneAssetLoaderContainer
{
public:
    ZoneAssetLoaderContainer(Zone& zone)
        : m_zone(zone)
    {
    }

    template<typename T> T& GetZoneAssetLoaderState()
    {
        static_assert(std::is_base_of_v<IZoneAssetLoaderState, T>, "T must inherit IZoneAssetLoaderState");
        // T must also have a public default constructor

        const auto foundEntry = m_zone_asset_loader_states.find(typeid(T));
        if (foundEntry != m_zone_asset_loader_states.end())
            return *dynamic_cast<T*>(foundEntry->second.get());

        auto newState = std::make_unique<T>();
        newState->SetZone(&m_zone);
        auto* newStatePtr = newState.get();
        m_zone_asset_loader_states.emplace(std::make_pair<std::type_index, std::unique_ptr<IZoneAssetLoaderState>>(typeid(T), std::move(newState)));

        return *newStatePtr;
    }

protected:
    Zone& m_zone;

private:
    std::unordered_map<std::type_index, std::unique_ptr<IZoneAssetLoaderState>> m_zone_asset_loader_states;
};
