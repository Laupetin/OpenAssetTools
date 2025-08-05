#pragma once

#include "Asset/AssetCreationContext.h"
#include "Asset/AssetCreationResult.h"
#include "InfoString/InfoString.h"

namespace phys_preset
{
    class InfoStringLoaderIW4
    {
    public:
        InfoStringLoaderIW4(MemoryManager& memory, Zone& zone);

        AssetCreationResult CreateAsset(const std::string& assetName, const InfoString& infoString, AssetCreationContext& context);

    private:
        MemoryManager& m_memory;
        Zone& m_zone;
    };
} // namespace phys_preset
