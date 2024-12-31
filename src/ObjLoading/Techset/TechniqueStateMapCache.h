#pragma once

#include "Asset/IZoneAssetLoaderState.h"
#include "StateMap/StateMapDefinition.h"
#include "Utils/ClassUtils.h"

#include <memory>
#include <string>
#include <unordered_map>

namespace techset
{
    class TechniqueStateMapCache final : public IZoneAssetLoaderState
    {
    public:
        _NODISCARD const state_map::StateMapDefinition* GetCachedStateMap(const std::string& name) const;
        void AddStateMapToCache(std::unique_ptr<state_map::StateMapDefinition> stateMap);

        _NODISCARD const state_map::StateMapDefinition* GetStateMapForTechnique(const std::string& techniqueName) const;
        void SetTechniqueUsesStateMap(std::string techniqueName, const state_map::StateMapDefinition* stateMap);

    private:
        std::unordered_map<std::string, const state_map::StateMapDefinition*> m_state_map_per_technique;
        std::unordered_map<std::string, std::unique_ptr<state_map::StateMapDefinition>> m_state_map_cache;
    };
} // namespace techset
