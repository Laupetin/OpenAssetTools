#include "RawLoaderSndAliasIW3.h"

#include "Game/IW3/IW3.h"
#include "Game/IW3/ObjConstantsIW3.h"
#include "InfoString/InfoString.h"
#include "CSVLoaderSndAliasIW3.h"
#include "Sound/SndAliasCommon.h"
#include "Utils/Logging/Log.h"

#include <format>
#include <iostream>

using namespace IW3;

namespace
{
    class RawLoaderSndAlias: public AssetCreator<AssetSound>
    {
    public:
        RawLoaderSndAlias(MemoryManager& memory, ISearchPath& searchPath, Zone& zone)
            : m_search_path(searchPath),
              m_info_string_loader(memory, zone)
        {
        }

        AssetCreationResult CreateAsset(const std::string& assetName, AssetCreationContext& context) override
        {
            const auto fileName = snd_alias::GetFileNameForAssetName(assetName);
            const auto file = m_search_path.Open(fileName);
            if (!file.IsOpen())
                return AssetCreationResult::NoAction();

            InfoString infoString;
            if (!infoString.FromStream(INFO_STRING_PREFIX_PHYS_PRESET, *file.m_stream))
            {
                con::error("Could not parse as info string file: \"{}\"", fileName);
                return AssetCreationResult::Failure();
            }

            return m_info_string_loader.CreateAsset(assetName, infoString, context);
        }

    private:
        ISearchPath& m_search_path;
        snd_alias::CSVLoaderIW3 m_info_string_loader;
    };
} // namespace

namespace snd_alias
{
    std::unique_ptr<AssetCreator<AssetSound>> CreateRawLoaderIW3(MemoryManager& memory, ISearchPath& searchPath, Zone& zone)
    {
        return std::make_unique<RawLoaderSndAlias>(memory, searchPath, zone);
    }
} // namespace phys_preset
