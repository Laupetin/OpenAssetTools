#include "RawLoaderAttachmentUniqueT6.h"

#include "Game/T6/ObjConstantsT6.h"
#include "Game/T6/T6.h"
#include "InfoString/InfoString.h"
#include "InfoStringLoaderAttachmentUniqueT6.h"

#include <cstring>
#include <format>
#include <iostream>

using namespace T6;

namespace
{
    class RawLoaderAttachmentUnique final : public AssetCreator<AssetAttachmentUnique>
    {
    public:
        RawLoaderAttachmentUnique(MemoryManager& memory, ISearchPath& searchPath, Zone& zone)
            : m_search_path(searchPath),
              m_info_string_loader(memory, searchPath, zone)
        {
        }

        AssetCreationResult CreateAsset(const std::string& assetName, AssetCreationContext& context) override
        {
            const auto fileName = std::format("attachmentunique/{}", assetName);
            const auto file = m_search_path.Open(fileName);
            if (!file.IsOpen())
                return AssetCreationResult::NoAction();

            InfoString infoString;
            if (!infoString.FromStream(ObjConstants::INFO_STRING_PREFIX_WEAPON_ATTACHMENT_UNIQUE, *file.m_stream))
            {
                std::cerr << std::format("Could not parse as info string file: \"{}\"\n", fileName);
                return AssetCreationResult::Failure();
            }

            return m_info_string_loader.CreateAsset(assetName, infoString, context);
        }

    private:
        ISearchPath& m_search_path;
        InfoStringLoaderAttachmentUnique m_info_string_loader;
    };
} // namespace

namespace T6
{
    std::unique_ptr<AssetCreator<AssetAttachmentUnique>> CreateRawAttachmentUniqueLoader(MemoryManager& memory, ISearchPath& searchPath, Zone& zone)
    {
        return std::make_unique<RawLoaderAttachmentUnique>(memory, searchPath, zone);
    }
} // namespace T6
