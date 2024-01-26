#include "AssetLoaderSoundBank.h"

#include "Csv/ParsedCsv.h"
#include "ObjContainer/SoundBank/SoundBankWriter.h"
#include "nlohmann/json.hpp"

#include "Game/T6/CommonT6.h"
#include "Game/T6/ObjConstantsT6.h"
#include "Game/T6/T6.h"
#include "Pool/GlobalAssetPool.h"

#include <Utils/StringUtils.h>
#include <cstring>
#include <iostream>
#include <fstream>
#include <climits>

using namespace T6;
namespace fs = std::filesystem;

namespace
{
    const std::string PREFIXES_TO_DROP[] {
        "raw/",
        "devraw/",
    };

    _NODISCARD std::string GetSoundFilePath(SndAlias* sndAlias)
    {
        std::string soundFilePath(sndAlias->assetFileName);

        std::replace(soundFilePath.begin(), soundFilePath.end(), '\\', '/');
        for (const auto& droppedPrefix : PREFIXES_TO_DROP)
        {
            if (soundFilePath.rfind(droppedPrefix, 0) != std::string::npos)
            {
                soundFilePath.erase(0, droppedPrefix.size());
                break;
            }
        }

        return soundFilePath;
    }

    _NODISCARD std::unique_ptr<std::ofstream> OpenSoundBankOutputFile(const std::string& bankName)
    {
        fs::path assetPath = SoundBankWriter::OutputPath / bankName;

        auto assetDir(assetPath);
        assetDir.remove_filename();

        create_directories(assetDir);

        auto outputStream = std::make_unique<std::ofstream>(assetPath, std::ios_base::out | std::ios_base::binary);

        if (outputStream->is_open())
        {
            return std::move(outputStream);
        }

        return nullptr;
    }
}

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

size_t GetValueIndex(const std::string& value, const std::string* lookupTable, size_t len)
{
    if (value.empty())
        return 0;

    for (auto i = 0u; i < len; i++)
    {
        if (lookupTable[i] == value)
            return i;
    }

    return 0;
}

unsigned int GetAliasSubListCount(unsigned int startRow, const ParsedCsv& csv)
{
    auto count = 1u;

    const auto name = csv[startRow].GetValue("name", true);
    if (name.empty())
        return 0;

    while (true)
    {
        if (startRow + count >= csv.Size())
            break;

        const auto testName = csv[startRow + count].GetValue("name", true);
        if (testName.empty())
            break;

        // if the name of the next entry does not match the first entry checked, it is not part of the sub list
        if (name != testName)
            break;

        count++;
    }

    return count;
}

bool LoadSoundAlias(MemoryManager* memory, SndAlias* alias, const ParsedCsvRow& row)
{
    memset(alias, 0, sizeof(SndAlias));

    const auto& name = row.GetValue("name", true);
    if (name.empty())
        return false;

    const auto& aliasFileName = row.GetValue("file", true);
    if (aliasFileName.empty())
        return false;

    alias->name = memory->Dup(name.data());
    alias->id = Common::SND_HashName(name.data());
    alias->assetFileName = memory->Dup(aliasFileName.data());
    alias->assetId = Common::SND_HashName(aliasFileName.data());

    auto secondaryName = row.GetValue("secondary");
    if (!secondaryName.empty())
        alias->secondaryname = memory->Dup(secondaryName.data());

    auto subtitle = row.GetValue("subtitle");
    if (!subtitle.empty())
        alias->subtitle = memory->Dup(subtitle.data());

    alias->duck = Common::SND_HashName(row.GetValue("duck").data());

    alias->volMin = row.GetValueInt<uint16_t>("vol_min");
    alias->volMax = row.GetValueInt<uint16_t>("vol_max");
    alias->distMin = row.GetValueInt<uint16_t>("dist_min");
    alias->distMax = row.GetValueInt<uint16_t>("dist_max");
    alias->distReverbMax = row.GetValueInt<uint16_t>("dist_reverb_max");
    alias->limitCount = row.GetValueInt<uint8_t>("limit_count");
    alias->entityLimitCount = row.GetValueInt<uint8_t>("entity_limit_count");
    alias->pitchMin = row.GetValueInt<uint16_t>("pitch_min");
    alias->pitchMax = row.GetValueInt<uint16_t>("pitch_max");
    alias->minPriority = row.GetValueInt<uint8_t>("min_priority");
    alias->maxPriority = row.GetValueInt<uint8_t>("max_priority");
    alias->minPriorityThreshold = row.GetValueInt<uint8_t>("min_priority_threshold");
    alias->maxPriorityThreshold = row.GetValueInt<uint8_t>("max_priority_threshold");
    alias->probability = row.GetValueInt<uint8_t>("probability");
    alias->startDelay = row.GetValueInt<uint16_t>("start_delay");
    alias->reverbSend = row.GetValueInt<uint16_t>("reverb_send");
    alias->centerSend = row.GetValueInt<uint16_t>("center_send");
    alias->envelopMin = row.GetValueInt<uint16_t>("envelop_min");
    alias->envelopMax = row.GetValueInt<uint16_t>("envelop_max");
    alias->envelopPercentage = row.GetValueInt<uint16_t>("envelop_percentage");
    alias->occlusionLevel = row.GetValueInt<uint8_t>("occlusion_level");
    alias->fluxTime = row.GetValueInt<uint16_t>("move_time");
    alias->futzPatch = row.GetValueInt<unsigned int>("futz");
    alias->contextType = row.GetValueInt<unsigned int>("context_type");
    alias->contextValue = row.GetValueInt<unsigned int>("context_value");
    alias->fadeIn = row.GetValueInt<int16_t>("fade_in");
    alias->fadeOut = row.GetValueInt<int16_t>("fade_out");

    alias->flags.looping = row.GetValue("loop") == "looping";
    alias->flags.panType = row.GetValue("pan") == "3d";
    alias->flags.isBig = row.GetValue("is_big") == "yes";
    alias->flags.distanceLpf = row.GetValue("distance_lpf") == "yes";
    alias->flags.doppler = row.GetValue("doppler") == "yes";
    alias->flags.timescale = row.GetValue("timescale") == "yes";
    alias->flags.isMusic = row.GetValue("music") == "yes";
    alias->flags.pauseable = row.GetValue("pause") == "yes";
    alias->flags.stopOnDeath = row.GetValue("stop_on_death") == "yes";

    alias->duckGroup =
        static_cast<char>(GetValueIndex(row.GetValue("duck_group"), ObjConstants::SOUND_DUCK_GROUPS.data(), ObjConstants::SOUND_DUCK_GROUPS.size()));
    alias->flags.volumeGroup = GetValueIndex(row.GetValue("group"), ObjConstants::SOUND_GROUPS.data(), ObjConstants::SOUND_GROUPS.size());
    alias->flags.fluxType = GetValueIndex(row.GetValue("move_type"), ObjConstants::SOUND_MOVE_TYPES.data(), ObjConstants::SOUND_MOVE_TYPES.size());
    alias->flags.loadType = GetValueIndex(row.GetValue("type"), ObjConstants::SOUND_LOAD_TYPES.data(), ObjConstants::SOUND_LOAD_TYPES.size());
    alias->flags.busType = GetValueIndex(row.GetValue("bus"), ObjConstants::SOUND_BUS_IDS.data(), ObjConstants::SOUND_BUS_IDS.size());
    alias->flags.limitType = GetValueIndex(row.GetValue("limit_type"), ObjConstants::SOUND_LIMIT_TYPES.data(), ObjConstants::SOUND_LIMIT_TYPES.size());
    alias->flags.volumeFalloffCurve = GetValueIndex(row.GetValue("volume_falloff_curve"), ObjConstants::SOUND_CURVES.data(), ObjConstants::SOUND_CURVES.size());
    alias->flags.reverbFalloffCurve = GetValueIndex(row.GetValue("reverb_falloff_curve"), ObjConstants::SOUND_CURVES.data(), ObjConstants::SOUND_CURVES.size());

    alias->flags.entityLimitType =
        GetValueIndex(row.GetValue("entity_limit_type"), ObjConstants::SOUND_LIMIT_TYPES.data(), ObjConstants::SOUND_LIMIT_TYPES.size());
    alias->flags.volumeMinFalloffCurve =
        GetValueIndex(row.GetValue("volume_min_falloff_curve"), ObjConstants::SOUND_CURVES.data(), ObjConstants::SOUND_CURVES.size());
    alias->flags.reverbMinFalloffCurve =
        GetValueIndex(row.GetValue("reverb_min_falloff_curve"), ObjConstants::SOUND_CURVES.data(), ObjConstants::SOUND_CURVES.size());
    alias->flags.randomizeType =
        GetValueIndex(row.GetValue("randomize_type"), ObjConstants::SOUND_RANDOMIZE_TYPES.data(), ObjConstants::SOUND_RANDOMIZE_TYPES.size());

    return true;
}

bool LoadSoundAliasIndexList(MemoryManager* memory, SndBank* sndBank)
{
    // contains a list of all the alias ids in the sound bank
    sndBank->aliasIndex = static_cast<SndIndexEntry*>(memory->Alloc(sizeof(SndIndexEntry) * sndBank->aliasCount));
    memset(sndBank->aliasIndex, 0xFF, sizeof(SndIndexEntry) * sndBank->aliasCount);

    bool* setAliasIndexList = new bool[sndBank->aliasCount];
    memset(setAliasIndexList, false, sndBank->aliasCount);

    for (auto i = 0u; i < sndBank->aliasCount; i++)
    {
        auto idx = sndBank->alias[i].id % sndBank->aliasCount;
        if (sndBank->aliasIndex[idx].value == USHRT_MAX)
        {
            sndBank->aliasIndex[idx].value = i;
            sndBank->aliasIndex[idx].next = USHRT_MAX;
            setAliasIndexList[i] = true;
        }
    }

    for (auto i = 0u; i < sndBank->aliasCount; i++)
    {
        if (setAliasIndexList[i])
            continue;

        auto idx = sndBank->alias[i].id % sndBank->aliasCount;
        while (sndBank->aliasIndex[idx].next != USHRT_MAX)
        {
            idx = sndBank->aliasIndex[idx].next;
        }

        auto offset = 1u;
        auto freeIdx = USHRT_MAX;
        while (true)
        {
            freeIdx = (idx + offset) % sndBank->aliasCount;
            if (sndBank->aliasIndex[freeIdx].value == USHRT_MAX)
                break;

            freeIdx = (idx + sndBank->aliasCount - offset) % sndBank->aliasCount;
            if (sndBank->aliasIndex[freeIdx].value == USHRT_MAX)
                break;

            offset++;
            freeIdx = USHRT_MAX;

            if (offset >= sndBank->aliasCount)
                break;
        }

        if (freeIdx == USHRT_MAX)
        {
            std::cerr << "Unable to allocate sound bank alias index list" << std::endl;
            delete[] setAliasIndexList;
            return false;
        }

        sndBank->aliasIndex[idx].next = freeIdx;
        sndBank->aliasIndex[freeIdx].value = i;
        sndBank->aliasIndex[freeIdx].next = USHRT_MAX;
        setAliasIndexList[i] = true;
    }

    delete[] setAliasIndexList;
    return true;
}

bool LoadSoundAliasList(
    MemoryManager* memory, SndBank* sndBank, const SearchPathOpenFile& file, unsigned int* loadedEntryCount, unsigned int* streamedEntryCount)
{
    const CsvInputStream aliasCsvStream(*file.m_stream);
    const ParsedCsv aliasCsv(aliasCsvStream, true);

    // Ensure there is at least one entry in the csv after the headers
    if (aliasCsv.Size() > 0)
    {
        // should be the total number of assets
        sndBank->aliasCount = aliasCsv.Size();
        sndBank->alias = static_cast<SndAliasList*>(memory->Alloc(sizeof(SndAliasList) * sndBank->aliasCount));
        memset(sndBank->alias, 0, sizeof(SndAliasList) * sndBank->aliasCount);

        auto row = 0u;
        auto listIndex = 0u;
        while (row < sndBank->aliasCount)
        {
            // count how many of the next rows should be in the sound alias sub-list. Aliases are part of the same sub list if they have the same name for a
            // different file
            auto subListCount = GetAliasSubListCount(row, aliasCsv);
            if (subListCount < 1)
                return false;

            // allocate the sub list
            sndBank->alias[listIndex].count = subListCount;
            sndBank->alias[listIndex].head = static_cast<SndAlias*>(memory->Alloc(sizeof(SndAlias) * subListCount));
            sndBank->alias[listIndex].sequence = 0;

            // populate the sublist with the next X number of aliases in the file. Note: this will only work correctly if the aliases that are a part of a sub
            // list are next to each other in the file
            for (auto i = 0u; i < subListCount; i++)
            {
                if (!LoadSoundAlias(memory, &sndBank->alias[listIndex].head[i], aliasCsv[row]))
                    return false;

                // if this asset is loaded instead of stream, increment the loaded count for later
                if (sndBank->alias[listIndex].head[i].flags.loadType == T6::SA_LOADED)
                    (*loadedEntryCount)++;
                else
                    (*streamedEntryCount)++;

                row++;
            }

            // the main alias list id and name should match that of the entries in the sub list (since they all have the same name, all sub entries will be the
            // same)
            sndBank->alias[listIndex].id = sndBank->alias[listIndex].head[0].id;
            sndBank->alias[listIndex].name = sndBank->alias[listIndex].head[0].name;

            listIndex++;
        }

        // re-allocate the alias list and count if necessary. We don't know the true aliasCount until after parsing all the aliases in the file
        if (listIndex != sndBank->aliasCount)
        {
            auto* oldAliases = sndBank->alias;

            sndBank->aliasCount = listIndex;
            sndBank->alias = static_cast<SndAliasList*>(memory->Alloc(sizeof(SndAliasList) * sndBank->aliasCount));
            memcpy(sndBank->alias, oldAliases, sizeof(SndAliasList) * sndBank->aliasCount);

            memory->Free(oldAliases);
        }

        if (!LoadSoundAliasIndexList(memory, sndBank))
            return false;
    }

    return true;
}

bool LoadSoundRadverbs(MemoryManager* memory, SndBank* sndBank, const SearchPathOpenFile& file)
{
    const CsvInputStream radverbCsvStream(*file.m_stream);
    const ParsedCsv radverbCsv(radverbCsvStream, true);

    if (radverbCsv.Size() > 0)
    {
        sndBank->radverbCount = radverbCsv.Size();
        sndBank->radverbs = static_cast<SndRadverb*>(memory->Alloc(sizeof(SndRadverb) * sndBank->radverbCount));
        memset(sndBank->radverbs, 0, sizeof(SndRadverb) * sndBank->radverbCount);

        for (auto i = 0u; i < sndBank->radverbCount; i++)
        {
            auto& row = radverbCsv[i];

            auto& name = row.GetValue("name", true);
            if (name.empty())
                return false;

            strncpy_s(sndBank->radverbs[i].name, name.data(), 32);
            sndBank->radverbs[i].id = Common::SND_HashName(name.data());
            sndBank->radverbs[i].smoothing = row.GetValueFloat("smoothing");
            sndBank->radverbs[i].earlyTime = row.GetValueFloat("earlyTime");
            sndBank->radverbs[i].lateTime = row.GetValueFloat("lateTime");
            sndBank->radverbs[i].earlyGain = row.GetValueFloat("earlyGain");
            sndBank->radverbs[i].lateGain = row.GetValueFloat("lateGain");
            sndBank->radverbs[i].returnGain = row.GetValueFloat("returnGain");
            sndBank->radverbs[i].earlyLpf = row.GetValueFloat("earlyLpf");
            sndBank->radverbs[i].lateLpf = row.GetValueFloat("lateLpf");
            sndBank->radverbs[i].inputLpf = row.GetValueFloat("inputLpf");
            sndBank->radverbs[i].dampLpf = row.GetValueFloat("dampLpf");
            sndBank->radverbs[i].wallReflect = row.GetValueFloat("wallReflect");
            sndBank->radverbs[i].dryGain = row.GetValueFloat("dryGain");
            sndBank->radverbs[i].earlySize = row.GetValueFloat("earlySize");
            sndBank->radverbs[i].lateSize = row.GetValueFloat("lateSize");
            sndBank->radverbs[i].diffusion = row.GetValueFloat("diffusion");
            sndBank->radverbs[i].returnHighpass = row.GetValueFloat("returnHighpass");
        }
    }

    return true;
}

bool LoadSoundDuckList(ISearchPath* searchPath, MemoryManager* memory, SndBank* sndBank, const SearchPathOpenFile& file)
{
    const CsvInputStream duckListCsvStream(*file.m_stream);
    const ParsedCsv duckListCsv(duckListCsvStream, true);

    if (duckListCsv.Size() > 0)
    {
        sndBank->duckCount = duckListCsv.Size();
        sndBank->ducks = static_cast<SndDuck*>(memory->Alloc(sizeof(SndDuck) * sndBank->duckCount));
        memset(sndBank->ducks, 0, sizeof(SndDuck) * sndBank->duckCount);

        for (auto i = 0u; i < sndBank->duckCount; i++)
        {
            auto* duck = &sndBank->ducks[i];
            auto& row = duckListCsv[i];
 
            const auto name = row.GetValue("name", true);
            if (name.empty())
                return false;

            const auto duckFile = searchPath->Open("soundbank/ducks/" + name + ".duk");
            if (!duckFile.IsOpen())
            {
                std::cerr << "Unable to find .duk file for " << name << " in ducklist for sound bank " << sndBank->name << std::endl;
                return false;
            }

            strncpy_s(duck->name, name.data(), 32);
            duck->id = Common::SND_HashName(name.data());

            auto duckJson = nlohmann::json::parse(*duckFile.m_stream);
            duck->fadeIn = duckJson["fadeIn"].get<float>();
            duck->fadeOut = duckJson["fadeOut"].get<float>();
            duck->startDelay = duckJson["startDelay"].get<float>();
            duck->distance = duckJson["distance"].get<float>();
            duck->length = duckJson["length"].get<float>();
            duck->updateWhilePaused = duckJson["updateWhilePaused"].get<int>();

            duck->fadeInCurve = duckJson["fadeInCurveId"].get<unsigned int>();
            duck->fadeOutCurve = duckJson["fadeOutCurveId"].get<unsigned int>();

            if (duckJson.contains("fadeInCurve"))
                duck->fadeInCurve = Common::SND_HashName(duckJson["fadeInCurve"].get<std::string>().data());

            if (duckJson.contains("fadeOutCurve"))
                duck->fadeOutCurve = Common::SND_HashName(duckJson["fadeOutCurve"].get<std::string>().data());

            duck->attenuation = static_cast<SndFloatAlign16*>(memory->Alloc(sizeof(SndFloatAlign16) * 32));
            duck->filter = static_cast<SndFloatAlign16*>(memory->Alloc(sizeof(SndFloatAlign16) * 32));

            for (auto& valueJson : duckJson["values"])
            {
                auto index =
                    GetValueIndex(valueJson["duckGroup"].get<std::string>(), ObjConstants::SOUND_DUCK_GROUPS.data(), ObjConstants::SOUND_DUCK_GROUPS.size());

                duck->attenuation[index] = valueJson["attenuation"].get<float>();
                duck->filter[index] = valueJson["filter"].get<float>();
            }
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

    // load the soundbank aliases
    unsigned int loadedEntryCount = 0u, streamedEntryCount = 0u;
    if (!LoadSoundAliasList(memory, sndBank, aliasFile, &loadedEntryCount, &streamedEntryCount))
        return false;

    // load the soundbank reverbs
    const auto radverbFile = searchPath->Open("soundbank/" + assetName + ".reverbs.csv");
    if (radverbFile.IsOpen())
    {
        if (!LoadSoundRadverbs(memory, sndBank, radverbFile))
        {
            std::cerr << "Sound Bank reverbs file for " << assetName << " is invalid" << std::endl;
            return false;
        }
    }

    // load the soundbank ducks
    const auto duckListFile = searchPath->Open("soundbank/" + assetName + ".ducklist.csv");
    if (duckListFile.IsOpen())
    {
        if (!LoadSoundDuckList(searchPath, memory, sndBank, duckListFile))
        {
            std::cerr << "Sound Bank ducklist file for " << assetName << " is invalid" << std::endl;
            return false;
        }
    }

    std::unique_ptr<std::ofstream> sablStream, sabsStream;
    std::unique_ptr<SoundBankWriter> sablWriter, sabsWriter;

    if (loadedEntryCount > 0)
    {
        sndBank->loadAssetBank.zone = memory->Dup(sndBankLocalization.at(0).c_str());
        sndBank->loadAssetBank.language = memory->Dup(sndBankLocalization.at(1).c_str());
        memset(sndBank->loadAssetBank.linkTimeChecksum, 0xCC, 16);

        sndBank->loadedAssets.loadedCount = 0;
        sndBank->loadedAssets.zone = memory->Dup(sndBankLocalization.at(0).c_str());
        sndBank->loadedAssets.language = memory->Dup(sndBankLocalization.at(1).c_str());
        sndBank->loadedAssets.entryCount = loadedEntryCount;
        sndBank->loadedAssets.entries = static_cast<SndAssetBankEntry*>(memory->Alloc(sizeof(SndAssetBankEntry) * loadedEntryCount));
        memset(sndBank->loadedAssets.entries, 0, sizeof(SndAssetBankEntry) * loadedEntryCount);

        const auto sablName = assetName + ".sabl";
        sablStream = OpenSoundBankOutputFile(sablName);
        if (sablStream)
            sablWriter = SoundBankWriter::Create(sablName, *sablStream, searchPath);
    }

    if (streamedEntryCount > 0)
    {
        sndBank->streamAssetBank.zone = memory->Dup(sndBankLocalization.at(0).c_str());
        sndBank->streamAssetBank.language = memory->Dup(sndBankLocalization.at(1).c_str());
        memset(sndBank->streamAssetBank.linkTimeChecksum, 0xCC, 16);

        const auto sabsName = assetName + ".sabs";
        sabsStream = OpenSoundBankOutputFile(sabsName);
        if (sabsStream)
            sablWriter = SoundBankWriter::Create(sabsName, *sabsStream, searchPath);
    }

    // add aliases to the correct sound bank writer
    for (auto i = 0u; i < sndBank->aliasCount; i++)
    {
        auto* aliasList = &sndBank->alias[i];
        for (auto j = 0; j < aliasList->count; j++)
        {
            auto* alias = &aliasList->head[j];

            if (sabsWriter && alias->flags.loadType == T6::SA_STREAMED)
                sabsWriter->AddSound(GetSoundFilePath(alias), alias->assetId);
            else if (sablWriter)
                sablWriter->AddSound(GetSoundFilePath(alias), alias->assetId);
        }
    }
    
    // write the output linked sound bank
    if (sablWriter)
    {
        sablWriter->Write();
        sablStream->close();
    }

    // write the output streamed sound bank
    if (sabsWriter)
    {
        sabsWriter->Write();
        sabsStream->close();
    }

    manager->AddAsset(ASSET_TYPE_SOUND, assetName, sndBank);
    return true;
}
