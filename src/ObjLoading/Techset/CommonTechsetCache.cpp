#include "CommonTechsetCache.h"

using namespace techset;

CommonTechset* CommonTechsetCache::GetCachedTechsetDefinition(const std::string& techsetName) const
{
    const auto foundTechset = m_cache.find(techsetName);

    if (foundTechset != m_cache.end())
        return foundTechset->second.get();

    return nullptr;
}

void CommonTechsetCache::AddCommonTechsetToCache(std::string name, std::unique_ptr<CommonTechset> definition)
{
    m_cache.emplace(std::make_pair(std::move(name), std::move(definition)));
}
