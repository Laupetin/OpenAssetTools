#include "AttachmentRawLoaderT6.h"

#include "AttachmentInfoStringLoaderT6.h"
#include "Game/T6/ObjConstantsT6.h"
#include "Game/T6/T6.h"
#include "InfoString/InfoString.h"
#include "Weapon/AttachmentCommon.h"

#include <cstring>
#include <format>
#include <iostream>

using namespace T6;
using namespace ::attachment;

namespace
{
    class RawLoaderAttachment final : public AssetCreator<AssetAttachment>
    {
    public:
        RawLoaderAttachment(MemoryManager& memory, ISearchPath& searchPath, Zone& zone)
            : m_search_path(searchPath),
              m_info_string_loader(memory, searchPath, zone)
        {
        }

        AssetCreationResult CreateAsset(const std::string& assetName, AssetCreationContext& context) override
        {
            const auto fileName = GetInfoStringFileNameForAssetName(assetName);
            const auto file = m_search_path.Open(fileName);
            if (!file.IsOpen())
                return AssetCreationResult::NoAction();

            InfoString infoString;
            if (!infoString.FromStream(ObjConstants::INFO_STRING_PREFIX_WEAPON_ATTACHMENT, *file.m_stream))
            {
                std::cerr << std::format("Could not parse as info string file: \"{}\"\n", fileName);
                return AssetCreationResult::Failure();
            }

            return m_info_string_loader.CreateAsset(assetName, infoString, context);
        }

    private:
        ISearchPath& m_search_path;
        attachment::InfoStringLoaderT6 m_info_string_loader;
    };
} // namespace

namespace attachment
{
    std::unique_ptr<AssetCreator<AssetAttachment>> CreateRawLoaderT6(MemoryManager& memory, ISearchPath& searchPath, Zone& zone)
    {
        return std::make_unique<RawLoaderAttachment>(memory, searchPath, zone);
    }
} // namespace attachment
