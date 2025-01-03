#pragma once

#include "Zone/Zone.h"

#include <memory>
#include <typeindex>
#include <unordered_map>

class IZoneAssetCreationState
{
protected:
    IZoneAssetCreationState() = default;

public:
    virtual ~IZoneAssetCreationState() = default;
    IZoneAssetCreationState(const IZoneAssetCreationState& other) = default;
    IZoneAssetCreationState(IZoneAssetCreationState&& other) noexcept = default;
    IZoneAssetCreationState& operator=(const IZoneAssetCreationState& other) = default;
    IZoneAssetCreationState& operator=(IZoneAssetCreationState&& other) noexcept = default;

    virtual void SetZone(Zone* zone)
    {
        // Do nothing by default
    }
};

class ZoneAssetCreationStateContainer
{
public:
    ZoneAssetCreationStateContainer(Zone& zone)
        : m_zone(zone)
    {
    }

    template<typename T> T& GetZoneAssetCreationState()
    {
        static_assert(std::is_base_of_v<IZoneAssetCreationState, T>, "T must inherit IZoneAssetCreationState");
        // T must also have a public default constructor

        const auto foundEntry = m_zone_asset_creation_states.find(typeid(T));
        if (foundEntry != m_zone_asset_creation_states.end())
            return *dynamic_cast<T*>(foundEntry->second.get());

        auto newState = std::make_unique<T>();
        newState->SetZone(&m_zone);
        auto* newStatePtr = newState.get();
        m_zone_asset_creation_states.emplace(std::make_pair<std::type_index, std::unique_ptr<IZoneAssetCreationState>>(typeid(T), std::move(newState)));

        return *newStatePtr;
    }

protected:
    Zone& m_zone;

private:
    std::unordered_map<std::type_index, std::unique_ptr<IZoneAssetCreationState>> m_zone_asset_creation_states;
};
