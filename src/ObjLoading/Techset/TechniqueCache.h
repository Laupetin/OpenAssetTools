#pragma once

#include "Asset/IZoneAssetCreationState.h"

#include <string>
#include <unordered_map>

namespace techset
{
    class TechniqueCache final : public IZoneAssetCreationState
    {
    public:
        template<typename TechniqueType> [[nodiscard]] TechniqueType* GetCachedTechnique(const std::string& techniqueName) const
        {
            const auto foundTechnique = m_cache.find(techniqueName);

            if (foundTechnique != m_cache.end())
                return static_cast<TechniqueType*>(foundTechnique->second);

            return nullptr;
        }

        void AddTechniqueToCache(std::string name, void* technique);

    private:
        std::unordered_map<std::string, void*> m_cache;
    };
} // namespace techset
