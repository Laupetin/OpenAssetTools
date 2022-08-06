#include "TechsetDefinitionCache.h"

using namespace techset;

TechsetDefinition* TechsetDefinitionCache::GetCachedTechsetDefinition(const std::string& techsetName) const
{
    const auto foundTechset = m_cache.find(techsetName);

    if (foundTechset != m_cache.end())
        return foundTechset->second.get();

    return nullptr;
}

void TechsetDefinitionCache::AddTechsetDefinitionToCache(std::string name, std::unique_ptr<TechsetDefinition> definition)
{
    m_cache.emplace(std::make_pair(std::move(name), std::move(definition)));
}
