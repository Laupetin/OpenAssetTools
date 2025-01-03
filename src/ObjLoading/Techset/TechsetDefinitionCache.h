#pragma once

#include "Asset/IZoneAssetCreationState.h"
#include "TechsetDefinition.h"
#include "Utils/ClassUtils.h"

#include <memory>
#include <string>
#include <unordered_map>

namespace techset
{
    class TechsetDefinitionCache final : public IZoneAssetCreationState
    {
    public:
        _NODISCARD TechsetDefinition* GetCachedTechsetDefinition(const std::string& techsetName) const;
        void AddTechsetDefinitionToCache(std::string name, std::unique_ptr<TechsetDefinition> definition);

    private:
        std::unordered_map<std::string, std::unique_ptr<TechsetDefinition>> m_cache;
    };
} // namespace techset
