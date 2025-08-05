#pragma once

#include "Asset/AssetCreationContext.h"
#include "Asset/AssetCreationResult.h"
#include "Game/T6/T6.h"
#include "InfoString/InfoString.h"

#include <vector>

namespace attachment_unique
{
    bool ExtractAttachmentsFromAssetNameT6(const std::string& assetName, std::vector<T6::eAttachment>& attachmentList);

    class InfoStringLoaderT6
    {
    public:
        InfoStringLoaderT6(MemoryManager& memory, ISearchPath& searchPath, Zone& zone);

        AssetCreationResult CreateAsset(const std::string& assetName, const InfoString& infoString, AssetCreationContext& context);

    private:
        MemoryManager& m_memory;
        ISearchPath& m_search_path;
        Zone& m_zone;
    };
} // namespace attachment_unique
