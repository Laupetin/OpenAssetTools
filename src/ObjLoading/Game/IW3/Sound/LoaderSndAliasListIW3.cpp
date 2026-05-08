#include "LoaderSndAliasListIW3.h"

#include "Game/IW3/IW3.h"
#include "Sound/SndAliasListCommon.h"
#include "Utils/Logging/Log.h"

#include <format>
#include <iostream>
#include <Csv/CsvStream.h>

using namespace IW3;

namespace
{
    constexpr unsigned COL_COUNT_MIN = 29;

    class SndAliasListLoader : public AssetCreator<AssetSound>
    {
    public:
        SndAliasListLoader(MemoryManager& memory, ISearchPath& searchPath, Zone& zone)
            : m_memory(memory)
            , m_search_path(searchPath)
        {
        }

        AssetCreationResult CreateAsset(const std::string& assetName, AssetCreationContext& context) override
        {
            const auto fileName = snd_alias::GetFileNameForAssetName(assetName);
            const auto file = m_search_path.Open(fileName);
            if (!file.IsOpen())
                return AssetCreationResult::NoAction();

            // TODO: Implement csv sound alias list loader

            return AssetCreationResult::NoAction();
        }

    private:
        ISearchPath& m_search_path;
    };
} // namespace

namespace snd_alias_list
{
    std::unique_ptr<AssetCreator<AssetSound>> CreateLoaderIW3(MemoryManager& memory, ISearchPath& searchPath, Zone& zone)
    {
        return std::make_unique<SndAliasListLoader>(memory, searchPath, zone);
    }
} // namespace snd_alias_list
