#include "AliasDumperJson.h"

#include "ObjContainer/SoundBank/SoundBankTypes.h"
#include "SoundCommon.h"

#include <iomanip>
#include <nlohmann/json.hpp>

using namespace nlohmann;

// #define PRETTY_PRINT_ALIAS 1

namespace T6::sound
{
    class AliasDumperJson final : public AbstractAliasDumper
    {
    public:
        explicit AliasDumperJson(const AssetDumpingContext& context)
            : AbstractAliasDumper(context)
        {
        }

    private:
        static void SetJsonString(json& j, const std::string& key, const char* value)
        {
            if (value)
                j[key] = value;
            else
                j[key] = nullptr;
        }

        static void AddAliasToJson(const SndAlias& alias, json& jAlias)
        {
            SoundAssetBankEntry entry;
            std::string extension;
            if (FindSoundBankEntry(alias.assetId, entry))
            {
                assert(entry.format < SND_ASSET_FORMAT_COUNT);
                if (entry.format < SND_ASSET_FORMAT_COUNT)
                    extension = EXTENSION_BY_FORMAT[entry.format];
            }

            SetJsonString(jAlias, "name", alias.name);
            jAlias["id"] = alias.id;
            SetJsonString(jAlias, "subtitle", alias.subtitle);
            SetJsonString(jAlias, "secondaryname", alias.secondaryname);
            jAlias["assetId"] = alias.assetId;

            if (alias.assetFileName)
                jAlias["assetFileName"] = GetAssetFilename(alias.assetFileName, extension);
            else
                jAlias["assetFileName"] = nullptr;

            jAlias["flags0"] = alias.flags0;
            jAlias["flags1"] = alias.flags1;
            jAlias["duck"] = alias.duck;
            jAlias["contextType"] = alias.contextType;
            jAlias["contextValue"] = alias.contextValue;
            jAlias["stopOnPlay"] = alias.stopOnPlay;
            jAlias["futzPatch"] = alias.futzPatch;
            jAlias["fluxTime"] = alias.fluxTime;
            jAlias["startDelay"] = alias.startDelay;
            jAlias["reverbSend"] = alias.reverbSend;
            jAlias["centerSend"] = alias.centerSend;
            jAlias["volMin"] = alias.volMin;
            jAlias["volMax"] = alias.volMax;
            jAlias["pitchMin"] = alias.pitchMin;
            jAlias["pitchMax"] = alias.pitchMax;
            jAlias["distMin"] = alias.distMin;
            jAlias["distMax"] = alias.distMax;
            jAlias["distReverbMax"] = alias.distReverbMax;
            jAlias["envelopMin"] = alias.envelopMin;
            jAlias["envelopMax"] = alias.envelopMax;
            jAlias["envelopPercentage"] = alias.envelopPercentage;
            jAlias["fadeIn"] = alias.fadeIn;
            jAlias["fadeOut"] = alias.fadeOut;
            jAlias["dopplerScale"] = alias.dopplerScale;
            jAlias["minPriorityThreshold"] = alias.minPriorityThreshold;
            jAlias["maxPriorityThreshold"] = alias.maxPriorityThreshold;
            jAlias["probability"] = alias.probability;
            jAlias["occlusionLevel"] = alias.occlusionLevel;
            jAlias["minPriority"] = alias.minPriority;
            jAlias["maxPriority"] = alias.maxPriority;
            jAlias["pan"] = alias.pan;
            jAlias["limitCount"] = alias.limitCount;
            jAlias["entityLimitCount"] = alias.entityLimitCount;
            jAlias["duckGroup"] = alias.duckGroup;
        }

        static void AddAliasListToJson(const SndAliasList& aliasList, json& jAliasList)
        {
            jAliasList["name"] = aliasList.name;
            jAliasList["id"] = aliasList.id;
            jAliasList["sequence"] = aliasList.sequence;

            json jAliases;
            for (auto j = 0; j < aliasList.count; j++)
            {
                json jAlias;

                AddAliasToJson(aliasList.head[j], jAlias);

                jAliases.push_back(std::move(jAlias));
            }
            jAliasList["aliases"] = std::move(jAliases);
        }

        void DumpSndBankAliases(const SndBank* sndBank) const override
        {
            const auto outputFile = OpenAliasOutputFile(sndBank, ".json");

            if (outputFile == nullptr)
            {
                std::cout << "Failed to open sound alias output file for: \"" << sndBank->name << "\"" << std::endl;
                return;
            }

            json jRoot;
            jRoot["version"] = 1;

            json jAliasLists = json::array();
            for (auto i = 0u; i < sndBank->aliasCount; i++)
            {
                json jAliasList;

                AddAliasListToJson(sndBank->alias[i], jAliasList);

                jAliasLists.push_back(std::move(jAliasList));
            }
            jRoot["aliasLists"] = std::move(jAliasLists);

            *outputFile
#ifdef PRETTY_PRINT_ALIAS
                << std::setw(4)
#endif
                << jRoot << std::endl;
        }
    };

    std::unique_ptr<AbstractAliasDumper> CreateAliasDumperJson(const AssetDumpingContext& context)
    {
        return std::make_unique<AliasDumperJson>(context);
    }
}