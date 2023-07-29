#pragma once

#include <string>
#include <ostream>
#include <memory>
#include <typeindex>

#include "IZoneDecompilingState.h"
#include "Utils/ClassUtils.h"
#include "Obj/Gdt/GdtStream.h"
#include "Zone/Zone.h"

class DecompilingContext
{
    std::unordered_map<std::type_index, std::unique_ptr<IZoneDecompilingState>> m_zone_decompiling_states;

public:
    Zone* m_zone;
    std::string m_base_path;
    std::unique_ptr<GdtOutputStream> m_gdt;

    DecompilingContext();

    _NODISCARD std::unique_ptr<std::ostream> OpenAssetFile(const std::string& fileName) const;

    template<typename T>
    T* GetZoneDecompilingState()
    {
        static_assert(std::is_base_of_v<IZoneDecompilingState, T>, "T must inherit IZoneDecompilingState");
        // T must also have a public default constructor

        const auto foundEntry = m_zone_decompiling_states.find(typeid(T));
        if (foundEntry != m_zone_decompiling_states.end())
            return dynamic_cast<T*>(foundEntry->second.get());

        auto newState = std::make_unique<T>();
        newState->SetZone(m_zone);
        auto* newStatePtr = newState.get();
        m_zone_decompiling_states.emplace(std::make_pair<std::type_index, std::unique_ptr<IZoneDecompilingState>>(typeid(T), std::move(newState)));
        return newStatePtr;
    }
};
