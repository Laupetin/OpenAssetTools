#pragma once

#include "Asset/AssetCreationContext.h"
#include "Asset/AssetCreationResult.h"
#include "Game/T6/T6.h"
#include "InfoString/InfoString.h"

#include <vector>

namespace T6
{
    class InfoStringLoaderAttachmentUnique
    {
    public:
        InfoStringLoaderAttachmentUnique(MemoryManager& memory, ISearchPath& searchPath, Zone& zone);

        AssetCreationResult CreateAsset(const std::string& assetName, const InfoString& infoString, AssetCreationContext& context);

        static bool ExtractAttachmentsFromAssetName(const std::string& assetName, std::vector<eAttachment>& attachmentList);

    private:
        MemoryManager& m_memory;
        ISearchPath& m_search_path;
        Zone& m_zone;
    };
} // namespace T6
