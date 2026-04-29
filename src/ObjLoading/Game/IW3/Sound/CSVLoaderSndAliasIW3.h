#pragma once

#include "Asset/AssetCreationContext.h"
#include "Asset/AssetCreationResult.h"
#include "InfoString/InfoString.h"

namespace snd_alias
{
    class CSVLoaderIW3
    {
    public:
        CSVLoaderIW3(MemoryManager& memory, Zone& zone);

        AssetCreationResult CreateAsset(const std::string& assetName, const InfoString& infoString, AssetCreationContext& context);

    private:
        MemoryManager& m_memory;
        Zone& m_zone;
    };
} // namespace snd_alias
