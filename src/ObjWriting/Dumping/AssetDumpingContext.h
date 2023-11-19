#pragma once

#include "IZoneAssetDumperState.h"
#include "Obj/Gdt/GdtStream.h"
#include "Utils/ClassUtils.h"
#include "Zone/Zone.h"

#include <memory>
#include <ostream>
#include <string>
#include <typeindex>

class AssetDumpingContext
{
    std::unordered_map<std::type_index, std::unique_ptr<IZoneAssetDumperState>> m_zone_asset_dumper_states;

public:
    Zone* m_zone;
    std::string m_base_path;
    std::unique_ptr<GdtOutputStream> m_gdt;

    AssetDumpingContext();

    _NODISCARD std::unique_ptr<std::ostream> OpenAssetFile(const std::string& fileName) const;

    template<typename T> T* GetZoneAssetDumperState()
    {
        static_assert(std::is_base_of_v<IZoneAssetDumperState, T>, "T must inherit IZoneAssetDumperState");
        // T must also have a public default constructor

        const auto foundEntry = m_zone_asset_dumper_states.find(typeid(T));
        if (foundEntry != m_zone_asset_dumper_states.end())
            return dynamic_cast<T*>(foundEntry->second.get());

        auto newState = std::make_unique<T>();
        newState->SetZone(m_zone);
        auto* newStatePtr = newState.get();
        m_zone_asset_dumper_states.emplace(std::make_pair<std::type_index, std::unique_ptr<IZoneAssetDumperState>>(typeid(T), std::move(newState)));
        return newStatePtr;
    }
};
