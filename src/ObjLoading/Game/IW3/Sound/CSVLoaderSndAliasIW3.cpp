#include "CSVLoaderSndAliasIW3.h"

#include "Game/IW3/IW3.h"
#include "Game/IW3/CSV/CSVToStructConverter.h"
#include "Game/IW3/Sound/SndAliasFields.h"
#include "Utils/Logging/Log.h"

#include <algorithm>
#include <cassert>
#include <cstring>
#include <format>
#include <iostream>
#include <limits>

using namespace IW3;

namespace
{
    class CSVToSndAliasConverter final : public CSVToStructConverter
    {
    public:
        CSVToSndAliasConverter(const InfoString& infoString,
                                        void* structure,
                                        ZoneScriptStrings& zoneScriptStrings,
                                        MemoryManager& memory,
                                        AssetCreationContext& context,
                                        GenericAssetRegistration& registration,
                                        const cspField_t* fields,
                                        size_t fieldCount)
            : CSVToStructConverter(infoString, structure, zoneScriptStrings, memory, context, registration, fields, fieldCount)
        {
        }

    protected:
        bool ConvertExtensionField(const cspField_t& field, const std::string& value) override
        {
            assert(false);
            return false;
        }
    };

    void CopyFromSndAliasInfo(const snd_alias_list_t_info& sndAliasListInfo, snd_alias_list_t& sndAliasList)
    {
        sndAliasList.aliasName = sndAliasListInfo.aliasName;
        sndAliasList.head = sndAliasListInfo.head;
        sndAliasList.count = sndAliasListInfo.count;
    }
} // namespace

namespace snd_alias
{
    CSVLoaderIW3::CSVLoaderIW3(MemoryManager& memory, Zone& zone)
        : m_memory(memory),
          m_zone(zone)
    {
    }

    AssetCreationResult CSVLoaderIW3::CreateAsset(const std::string& assetName, const InfoString& infoString, AssetCreationContext& context)
    {
        snd_alias_list_t_info sndAliasListInfo;
        std::memset(&sndAliasListInfo, 0, sizeof(sndAliasListInfo));

        auto* physPreset = m_memory.Alloc<snd_alias_list_t>();
        AssetRegistration<AssetSound> registration(assetName, physPreset);

        CSVToSndAliasConverter converter(infoString,
                                         &sndAliasListInfo,
                                         m_zone.m_script_strings,
                                         m_memory,
                                         context,
                                         registration,
                                         snd_alias_fields,
                                         std::extent_v<decltype(snd_alias_fields)>);
        if (!converter.Convert())
        {
            con::error("Failed to parse sound alias list: \"{}\"", assetName);
            return AssetCreationResult::Failure();
        }

        CopyFromSndAliasInfo(sndAliasListInfo, *physPreset);
        physPreset->aliasName = m_memory.Dup(assetName.c_str());

        return AssetCreationResult::Success(context.AddAsset(std::move(registration)));
    }
} // namespace phys_preset
