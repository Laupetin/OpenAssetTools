#pragma once

#include "Asset/AssetCreationContext.h"
#include "Asset/AssetCreationResult.h"
#include "InfoString/InfoString.h"

namespace T6::phys_preset
{
    class InfoStringLoader
    {
    public:
        InfoStringLoader(MemoryManager& memory, Zone& zone);

        AssetCreationResult CreateAsset(const std::string& assetName, const InfoString& infoString, AssetCreationContext& context);

    private:
        MemoryManager& m_memory;
        Zone& m_zone;
    };
} // namespace T6::phys_preset
