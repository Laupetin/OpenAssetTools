#pragma once

#include <unordered_map>
#include <string>
#include <memory>

#include "Utils/ClassUtils.h"
#include "TechsetDefinition.h"
#include "AssetLoading/IZoneAssetLoaderState.h"

namespace techset
{
    class TechsetDefinitionCache final : public IZoneAssetLoaderState
    {
    public:
        _NODISCARD TechsetDefinition* GetCachedTechsetDefinition(const std::string& techsetName) const;
        void AddTechsetDefinitionToCache(std::string name, std::unique_ptr<TechsetDefinition> definition);

    private:
        std::unordered_map<std::string, std::unique_ptr<TechsetDefinition>> m_cache;
    };
}
