#include "TechniqueCache.h"

namespace techset
{
    void TechniqueCache::AddTechniqueToCache(std::string name, void* technique)
    {
        m_cache.emplace(std::move(name), technique);
    }
} // namespace techset
