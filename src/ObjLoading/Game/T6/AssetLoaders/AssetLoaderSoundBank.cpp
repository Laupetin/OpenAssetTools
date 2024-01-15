#include "AssetLoaderSoundBank.h"

#include "Csv/CsvStream.h"
#include "Game/T6/CommonT6.h"
#include "Game/T6/ObjConstantsT6.h"
#include "Game/T6/T6.h"
#include "Pool/GlobalAssetPool.h"

#include <Utils/StringUtils.h>
#include <cstring>
#include <iostream>

using namespace T6;
std::unordered_map<std::string, unsigned int> AliasHeaders{};

void* AssetLoaderSoundBank::CreateEmptyAsset(const std::string& assetName, MemoryManager* memory)
{
    auto* soundBank = memory->Create<SndBank>();
    memset(soundBank, 0, sizeof(SndBank));
    soundBank->name = memory->Dup(assetName.c_str());
    return soundBank;
}

bool AssetLoaderSoundBank::CanLoadFromRaw() const
{
    return true;
}

void LoadSoundAliasHeader(const std::vector<std::string>& values)
{
    for (auto i = 0u; i < values.size(); i++)
    {
        AliasHeaders[values[i]] = i;
    }
}

const char* GetSoundAliasValue(const std::string& header, const std::vector<std::string>& values, bool required = false)
{
    if (AliasHeaders.find(header) == AliasHeaders.end())
    {
        if (required)
            std::cerr << "ERROR: Required column \"" << header << "\" was not found";
        else
            std::cerr << "WARNING: Expected column \"" << header << "\" was not found";
        return nullptr;
    }

    auto value = values.at(AliasHeaders[header]).c_str();
    if (required && (!value || !*value))
    {
        std::cerr << "ERROR: Required column \"" << header << "\" does not have a value";
        return nullptr;
    }

    return value;
}

char* GetSoundAliasValueString(const std::string& header, const std::vector<std::string>& values, MemoryManager* memory, bool required = false)
{
    const auto* value = GetSoundAliasValue(header, values, required);
    return value ? memory->Dup(value) : nullptr;
}

int32_t GetSoundAliasValueInt(const std::string& header, const std::vector<std::string>& values, bool required = false)
{
    const auto* value = GetSoundAliasValue(header, values, required);
    if (value && *value)
        return std::stoi(value);
    return 0;
}

size_t GetSoundAliasValueIndex(
    const std::string& header, const std::vector<std::string>& values, const std::string* lookupTable, size_t len, bool required = false)
{
    const auto* value = GetSoundAliasValue(header, values, required);
    if (!value || !*value)
        return 0;

    for (auto i = 0u; i < len; i++)
    {
        if (lookupTable[i] == value)
            return i;
    }

    return 0;
}

bool GetSoundAliasValueBool(const std::string& header, const std::vector<std::string>& values, const std::string& comparison, bool required = false)
{
    const auto* value = GetSoundAliasValue(header, values, required);
    if (value && *value)
        return comparison == value;
    return false;
}

bool LoadSoundAlias(MemoryManager* memory, SndAlias* alias, const std::vector<std::string>& values)
{
    memset(alias, 0, sizeof(SndAlias));

    const auto* name = GetSoundAliasValue("name", values, true);
    if (name == nullptr)
        return false;

    const auto* aliasFileName = GetSoundAliasValue("file", values, true);
    if (aliasFileName == nullptr)
        return false;

    alias->name = memory->Dup(name);
    alias->id = Common::SND_HashName(name);
    alias->assetFileName = memory->Dup(aliasFileName);
    alias->assetId = Common::SND_HashName(aliasFileName);
    alias->secondaryname = GetSoundAliasValueString("secondary", values, memory);
    alias->subtitle = GetSoundAliasValueString("subtitle", values, memory);

    alias->duck = Common::SND_HashName(GetSoundAliasValue("duck", values));

    alias->volMin = GetSoundAliasValueInt("vol_min", values);
    alias->volMax = GetSoundAliasValueInt("vol_max", values);
    alias->distMin = GetSoundAliasValueInt("dist_min", values);
    alias->distMax = GetSoundAliasValueInt("dist_max", values);
    alias->distReverbMax = GetSoundAliasValueInt("dist_reverb_max", values);
    alias->limitCount = GetSoundAliasValueInt("limit_count", values);
    alias->entityLimitCount = GetSoundAliasValueInt("entity_limit_count", values);
    alias->pitchMin = GetSoundAliasValueInt("pitch_min", values);
    alias->pitchMax = GetSoundAliasValueInt("pitch_max", values);
    alias->minPriority = GetSoundAliasValueInt("min_priority", values);
    alias->maxPriority = GetSoundAliasValueInt("max_priority", values);
    alias->minPriorityThreshold = GetSoundAliasValueInt("min_priority_threshold", values);
    alias->maxPriorityThreshold = GetSoundAliasValueInt("max_priority_threshold", values);
    alias->probability = GetSoundAliasValueInt("probability", values);
    alias->startDelay = GetSoundAliasValueInt("start_delay", values);
    alias->reverbSend = GetSoundAliasValueInt("reverb_send", values);
    alias->centerSend = GetSoundAliasValueInt("center_send", values);
    alias->envelopMin = GetSoundAliasValueInt("envelop_min", values);
    alias->envelopMax = GetSoundAliasValueInt("envelop_max", values);
    alias->envelopPercentage = GetSoundAliasValueInt("envelop_percentage", values);
    alias->occlusionLevel = GetSoundAliasValueInt("occlusion_level", values);
    alias->fluxTime = GetSoundAliasValueInt("move_time", values);
    alias->futzPatch = GetSoundAliasValueInt("futz", values);
    alias->contextType = GetSoundAliasValueInt("context_type", values);
    alias->contextValue = GetSoundAliasValueInt("context_value", values);
    alias->fadeIn = GetSoundAliasValueInt("fade_in", values);
    alias->fadeOut = GetSoundAliasValueInt("fade_out", values);

    alias->flags.looping = GetSoundAliasValueBool("loop", values, "looping");
    alias->flags.panType = GetSoundAliasValueBool("pan", values, "3d");
    alias->flags.isBig = GetSoundAliasValueBool("is_big", values, "yes");
    alias->flags.distanceLpf = GetSoundAliasValueBool("distance_lpf", values, "yes");
    alias->flags.doppler = GetSoundAliasValueBool("doppler", values, "yes");
    alias->flags.timescale = GetSoundAliasValueBool("timescale", values, "yes");
    alias->flags.isMusic = GetSoundAliasValueBool("music", values, "yes");
    alias->flags.pauseable = GetSoundAliasValueBool("pause", values, "yes");
    alias->flags.stopOnDeath = GetSoundAliasValueBool("stop_on_death", values, "yes");
    
    alias->flags.volumeGroup = GetSoundAliasValueIndex("group", values, ObjConstants::SOUND_GROUPS.data(), ObjConstants::SOUND_GROUPS.size());
    alias->flags.fluxType = GetSoundAliasValueIndex("move_type", values, ObjConstants::SOUND_MOVE_TYPES.data(), ObjConstants::SOUND_MOVE_TYPES.size());
    alias->flags.loadType = GetSoundAliasValueIndex("type", values, ObjConstants::SOUND_LOAD_TYPES.data(), ObjConstants::SOUND_LOAD_TYPES.size());
    alias->flags.busType = GetSoundAliasValueIndex("bus", values, ObjConstants::SOUND_BUS_IDS.data(), ObjConstants::SOUND_BUS_IDS.size());
    alias->flags.limitType = GetSoundAliasValueIndex("limit_type", values, ObjConstants::SOUND_LIMIT_TYPES.data(), ObjConstants::SOUND_LIMIT_TYPES.size());
    alias->flags.entityLimitType =
        GetSoundAliasValueIndex("entity_limit_type", values, ObjConstants::SOUND_LIMIT_TYPES.data(), ObjConstants::SOUND_LIMIT_TYPES.size());
    alias->flags.volumeFalloffCurve =
        GetSoundAliasValueIndex("volume_falloff_curve", values, ObjConstants::SOUND_CURVES.data(), ObjConstants::SOUND_CURVES.size());
    alias->flags.reverbFalloffCurve =
        GetSoundAliasValueIndex("reverb_falloff_curve", values, ObjConstants::SOUND_CURVES.data(), ObjConstants::SOUND_CURVES.size());
    alias->flags.volumeMinFalloffCurve =
        GetSoundAliasValueIndex("volume_min_falloff_curve", values, ObjConstants::SOUND_CURVES.data(), ObjConstants::SOUND_CURVES.size());
    alias->flags.reverbMinFalloffCurve =
        GetSoundAliasValueIndex("reverb_min_falloff_curve", values, ObjConstants::SOUND_CURVES.data(), ObjConstants::SOUND_CURVES.size());
    alias->flags.randomizeType =
        GetSoundAliasValueIndex("type", values, ObjConstants::SOUND_RANDOMIZE_TYPES.data(), ObjConstants::SOUND_RANDOMIZE_TYPES.size());

    return true;
}

unsigned int GetAliasSubListCount(unsigned int startRow, std::vector<std::vector<std::string>>& csvLines)
{
    auto count = 1u;

    const auto* name = GetSoundAliasValue("name", csvLines[startRow], true);
    if (!name || !*name)
        return 0;

    while (true)
    {
        if (startRow + count >= csvLines.size())
            break;

        const auto* testName = GetSoundAliasValue("name", csvLines[startRow + count], true);
        if (!name || !*name)
            break;

        // if the name of the next entry does not match the first entry checked, it is not part of the sub list
        if (strcmp(name, testName) != 0)
            break;

        count++;
    }

    return count;
}

bool LoadSoundAliasList(MemoryManager* memory, SndBank* sndBank, const SearchPathOpenFile& file, unsigned int* loadedEntryCount)
{
    const CsvInputStream aliasCsv(*file.m_stream);
    std::vector<std::vector<std::string>> csvLines;
    std::vector<std::string> currentLine;
    auto maxCols = 0u;

    while (aliasCsv.NextRow(currentLine))
    {
        if (currentLine.size() > maxCols)
            maxCols = currentLine.size();
        csvLines.emplace_back(std::move(currentLine));
        currentLine = std::vector<std::string>();
    }

    // Ensure there is at least one entry in the csv after the headers
    if (maxCols * csvLines.size() > maxCols)
    {
        // should be the total number of assets
        sndBank->aliasCount = csvLines.size() - 1;
        sndBank->alias = static_cast<SndAliasList*>(memory->Alloc(sizeof(SndAliasList) * sndBank->aliasCount));
        memset(sndBank->alias, 0, sizeof(SndAliasList) * sndBank->aliasCount);

        // contains a list of all the alias ids in the sound bank
        sndBank->aliasIndex = static_cast<SndIndexEntry*>(memory->Alloc(sizeof(SndIndexEntry) * sndBank->aliasCount));
        memset(sndBank->aliasIndex, 0, sizeof(SndIndexEntry) * sndBank->aliasCount);

        LoadSoundAliasHeader(csvLines[0]);

        auto row = 1u;
        auto listIndex = 0u;
        while (row < csvLines.size())
        {
            // count how many of the next rows should be in the sound alias sub-list. Aliases are part of the same sub list if they have the same name for a different file
            auto subListCount = GetAliasSubListCount(row, csvLines);
            if (subListCount < 1)
                return false;

            // allocate the sub list
            sndBank->alias[listIndex].count = subListCount;
            sndBank->alias[listIndex].head = static_cast<SndAlias*>(memory->Alloc(sizeof(SndAlias) * subListCount));
            sndBank->alias[listIndex].sequence = 0;

            // populate the sublist with the next X number of aliases in the file. Note: this will only work correctly if the aliases that are a part of a sub list are next to each other in the file
            for (auto i = 0; i < subListCount; i++)
            {
                const auto& aliasValues = csvLines[row];
                if (!LoadSoundAlias(memory, &sndBank->alias[listIndex].head[i], aliasValues))
                    return false;

                // if this asset is loaded instead of stream, increment the loaded count for later
                if (sndBank->alias[listIndex].head[i].flags.loadType == T6::SA_LOADED)
                    (*loadedEntryCount)++;

                // populate the aliasIndex array
                sndBank->aliasIndex[row - 1].value = sndBank->alias[listIndex].head[i].id;
                sndBank->aliasIndex[row - 1].next = 0xFFFF;

                row++;
            }

            // the main alias list id and name should match that of the entries in the sub list (since they all have the same name, all sub entries will be the same)
            sndBank->alias[listIndex].id = sndBank->alias[listIndex].head[0].id;
            sndBank->alias[listIndex].name = sndBank->alias[listIndex].head[0].name;

            listIndex++;
        }
    }

    return true;
}

bool AssetLoaderSoundBank::LoadFromRaw(
    const std::string& assetName, ISearchPath* searchPath, MemoryManager* memory, IAssetLoadingManager* manager, Zone* zone) const
{
    if (assetName.find('.') == std::string::npos)
    {
        std::cerr << "A language must be specific in the soundbank asset name! (Ex: mpl_common.all)" << std::endl;
        return false;
    }

    // open the soundbank aliases
    const auto aliasFile = searchPath->Open("soundbank/" + assetName + ".aliases.csv");
    if (!aliasFile.IsOpen())
        return false;

    // set the defaults
    auto* sndBank = memory->Create<SndBank>();
    memset(sndBank, 0, sizeof(SndBank));

    sndBank->name = memory->Dup(assetName.c_str());

    auto sndBankLocalization = utils::StringSplit(assetName, '.');
    sndBank->streamAssetBank.zone = memory->Dup(sndBankLocalization.at(0).c_str());
    sndBank->streamAssetBank.language = memory->Dup(sndBankLocalization.at(1).c_str());
    memset(sndBank->streamAssetBank.linkTimeChecksum, 0xCC, 16);

    sndBank->loadAssetBank.zone = memory->Dup(sndBankLocalization.at(0).c_str());
    sndBank->loadAssetBank.language = memory->Dup(sndBankLocalization.at(1).c_str());
    memset(sndBank->loadAssetBank.linkTimeChecksum, 0xCC, 16);

    sndBank->loadedAssets.zone = memory->Dup(sndBankLocalization.at(0).c_str());
    sndBank->loadedAssets.language = memory->Dup(sndBankLocalization.at(1).c_str());
    sndBank->loadedAssets.loadedCount = 0;

    unsigned int loadedEntryCount = 0u;
    if (!LoadSoundAliasList(memory, sndBank, aliasFile, &loadedEntryCount))
        return false;

    // allocate memory for the number of non-streamed assets
    sndBank->loadedAssets.entryCount = loadedEntryCount;
    sndBank->loadedAssets.entries = static_cast<SndAssetBankEntry*>(memory->Alloc(sizeof(SndAssetBankEntry) * loadedEntryCount));
    memset(sndBank->loadedAssets.entries, 0, sizeof(SndAssetBankEntry) * loadedEntryCount);

    // open the soundbank reverbs
    sndBank->radverbs = nullptr;
    sndBank->radverbCount = 0;

    // open the soundbank ducks
    sndBank->ducks = nullptr;
    sndBank->duckCount = 0;

    manager->AddAsset(ASSET_TYPE_SOUND, assetName, sndBank);
    return true;
}
