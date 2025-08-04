#pragma once

#include "Asset/AssetCreationContext.h"
#include "Asset/AssetCreationResult.h"
#include "InfoString/InfoString.h"

namespace IW5::weapon
{
    class InfoStringLoader
    {
    public:
        InfoStringLoader(MemoryManager& memory, ISearchPath& searchPath, Zone& zone);

        AssetCreationResult CreateAsset(const std::string& assetName, const InfoString& infoString, AssetCreationContext& context) const;

    private:
        MemoryManager& m_memory;
        ISearchPath& m_search_path;
        Zone& m_zone;
    };
} // namespace IW5::weapon
