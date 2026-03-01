#include "TechniqueStateMapCache.h"

using namespace techset;

const state_map::StateMapDefinition* TechniqueStateMapCache::GetCachedStateMap(const std::string& name) const
{
    const auto foundStateMap = m_state_map_cache.find(name);

    if (foundStateMap != m_state_map_cache.end())
        return foundStateMap->second.get();

    return nullptr;
}

void TechniqueStateMapCache::AddStateMapToCache(std::unique_ptr<state_map::StateMapDefinition> stateMap)
{
    m_state_map_cache.emplace(std::make_pair(stateMap->m_name, std::move(stateMap)));
}

const state_map::StateMapDefinition* TechniqueStateMapCache::GetStateMapForTechnique(const std::string& techniqueName) const
{
    const auto foundTechnique = m_state_map_per_technique.find(techniqueName);

    if (foundTechnique != m_state_map_per_technique.end())
        return foundTechnique->second;

    return nullptr;
}

void TechniqueStateMapCache::SetTechniqueUsesStateMap(std::string techniqueName, const state_map::StateMapDefinition* stateMap)
{
    m_state_map_per_technique.emplace(std::make_pair(std::move(techniqueName), stateMap));
}
