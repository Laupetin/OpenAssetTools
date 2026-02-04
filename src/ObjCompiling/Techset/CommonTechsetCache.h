#pragma once

#include "Asset/IZoneAssetCreationState.h"
#include "Techset/CommonTechset.h"

#include <memory>
#include <string>
#include <unordered_map>

namespace techset
{
    class CommonTechsetCache final : public IZoneAssetCreationState
    {
    public:
        [[nodiscard]] CommonTechset* GetCachedTechsetDefinition(const std::string& techsetName) const;
        void AddCommonTechsetToCache(std::string name, std::unique_ptr<CommonTechset> definition);

    private:
        std::unordered_map<std::string, std::unique_ptr<CommonTechset>> m_cache;
    };
} // namespace techset
