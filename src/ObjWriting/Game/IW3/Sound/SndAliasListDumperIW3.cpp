#include "SndAliasListDumperIW3.h"

#include "Csv/CsvStream.h"
#include "Game/IW3/Sound/SndAliasListConstantsIW3.h"
#include "Sound/WavWriter.h"
#include "Sound/SndAliasListCommon.h"
#include "Utils/Logging/Log.h"
#include <Sound/SndAliasListDumper.h>

#include <algorithm>
#include <filesystem>
#include <format>
#include <sstream>

using namespace IW3;
using namespace snd_alias_list;

namespace
{
    void WriteAliasToFile(SndAliasListDumper& dumper, const snd_alias_t& alias, int sequence = 0)
    {
        dumper.WriteColumnString(alias.aliasName);
        dumper.WriteColumnIntegral((alias.sequence ? alias.sequence : sequence), 0);

        if (alias.soundFile)
        {
            if (alias.soundFile->type == SAT_STREAMED && alias.soundFile->u.streamSnd.dir && alias.soundFile->u.streamSnd.name)
            {
                dumper.WriteColumnString(std::format("{}/{}", alias.soundFile->u.streamSnd.dir, alias.soundFile->u.streamSnd.name));
            }
            else if (alias.soundFile->type == SAT_LOADED && alias.soundFile->u.loadSnd->name)
            {
                dumper.WriteColumnString(FromReferencedString(alias.soundFile->u.loadSnd->name));
            }
        }

        dumper.WriteColumnVolumeLinear(alias.volMin);
        dumper.WriteColumnVolumeLinear(alias.volMax);
        dumper.WriteColumnString("max");
        dumper.WriteColumnFloat(alias.pitchMin, 1.0f);
        dumper.WriteColumnFloat(alias.pitchMax, 1.0f);
        dumper.WriteColumnFloat(alias.distMin, 120.0f);
        dumper.WriteColumnFloat(alias.distMax, 500000.0f);
        dumper.WriteChannelEnum((alias.flags >> 8) & 0x3F, snd_alias_channel_names, std::extent_v<decltype(snd_alias_channel_names)>);

        if ((alias.flags & 0x40) && (alias.flags & 0x80))
        {
            // Not sure if both flags set means primed
            dumper.WriteColumnString("primed");
        }
        else if (alias.flags & 0x80 && !(alias.flags & 0x40))
        {
            dumper.WriteColumnString("streamed");
        }
        else
        {
            dumper.WriteColumnString("");
        }

        dumper.WriteColumnFloat(alias.probability, 1.0f);

        if (alias.flags & 0x1)
        {
            if (alias.flags & 0x20)
            {
                dumper.WriteColumnString("rlooping");
            }
            else
            {
                dumper.WriteColumnString("looping");
            }
        }
        else
        {
            dumper.WriteColumnString("");
        }

        // masterslave
        if (alias.flags & 0x2)
        {
            dumper.WriteColumnString("master");
        }
        else
        {
            dumper.WriteColumnFloat(alias.slavePercentage, 1.0f);
        }

        dumper.WriteColumnString("");

        if (alias.subtitle)
        {
            std::string subtitle = "";
            std::istringstream subtitleStream(alias.subtitle);
            getline(subtitleStream, subtitle, ',');
            dumper.WriteColumnString(subtitle);
        }
        else
        {
            dumper.WriteColumnString("");
        }

        dumper.WriteColumnString("");
        dumper.WriteColumnString(alias.secondaryAliasName);

        if (alias.volumeFalloffCurve)
        {
            dumper.WriteColumnString(alias.volumeFalloffCurve->filename);
        }

        dumper.WriteColumnIntegral(alias.startDelay);

        if (alias.speakerMap)
        {
            dumper.WriteColumnString(alias.speakerMap->name);
        }

        // reverb
        std::string reverbString = "";
        if (alias.flags & 0x10)
        {
            reverbString += "nowetlevel";
            if (alias.flags & 0x8)
            {
                reverbString += " ";
            }
        }
        if (alias.flags & 0x8)
        {
            reverbString += "fulldrylevel";
        }
        dumper.WriteColumnString(reverbString);
        dumper.WriteColumnFloat(alias.lfePercentage, 0);
        dumper.WriteColumnFloat(alias.centerPercentage, 0);
        dumper.WriteColumnString("");
        dumper.WriteColumnFloat(alias.envelopMin, 0);
        dumper.WriteColumnFloat(alias.envelopMax, 0);
        dumper.WriteColumnFloat(alias.envelopPercentage, 0);
        dumper.WriteColumnString("");
    }

    const char* SourceName(int numLevels, int levelIndex)
    {
        if (numLevels == 1)
            return "MONOSOURCE";
        return (levelIndex == 0) ? "LEFTSOURCE" : "RIGHTSOURCE";
    }

    std::string IndentedSpeakerName(const std::string& speakerName)
    {
        if (speakerName.size() <= 11)
        {
            return speakerName + "\t\t\t";
        }
        else if (speakerName.size() <= 13)
        {
            return speakerName + "\t\t";
        }
        else if (speakerName.size() <= 20)
        {
            return speakerName + "\t";
        }
        return speakerName;
    }

    void DumpSpeakerMap(const AssetDumpingContext& context, const SpeakerMap* speakerMap)
    {
        if (!speakerMap || !speakerMap->name || speakerMap->name[0] == '\0')
            return;

        const auto outFile = OpenAssetOutputFile(context.m_base_path, std::format("soundaliases/{}", speakerMap->name), ".spkrmap");
        if (!outFile)
        {
            con::error("Failed to open soundspeaker map output file: \"{}\"", speakerMap->name);
            return;
        }
        std::ostream& stream(*outFile);
        stream << "SPKRMAP\r\n";

        for (int j = 0; j < std::extent_v<std::remove_reference_t<decltype(speakerMap->channelMaps[0])>>; ++j)
        {
            for (int i = 0; i < std::extent_v<decltype(speakerMap->channelMaps)>; ++i)
            {
                stream << "\r\n\r\n";

                auto& channelMap = speakerMap->channelMaps[i][j];
                for (int k = 0; k < std::min(channelMap.speakerCount, (int)std::extent_v<decltype(channelMap.speakers)>); ++k)
                {
                    auto& speakers = channelMap.speakers[k];
                    for (int l = 0; l < speakers.numLevels; ++l)
                    {
                        stream << SourceName(speakers.numLevels, l) << "\t\t" << IndentedSpeakerName(speaker_map_names[speakers.speaker]) << '\t' << std::fixed
                               << std::setprecision(4) << speakers.levels[l] << "\r\n";
                    }
                }
            }
        }
    }

    void DumpAliases(const AssetDumpingContext& context, const snd_alias_list_t& sndAliasList)
    {
        const auto outFile = OpenAssetOutputFile(context.m_base_path, std::format("soundaliases/{}", sndAliasList.aliasName), ".csv");
        if (!outFile)
        {
            con::error("Failed to open sound alias list output file: \"\"", sndAliasList.aliasName);
            return;
        }

        SndAliasListDumper dumper(*outFile);
        dumper.WriteAliasFileHeader(sound_alias_headers, std::extent_v<decltype(sound_alias_headers)>);

        for (int i = 0; i < sndAliasList.count; i++)
        {
            const auto& alias = sndAliasList.head[i];

            if (sndAliasList.count == 1)
            {
                WriteAliasToFile(dumper, alias);
            }
            else
            {
                WriteAliasToFile(dumper, alias, i + 1);
            }

            dumper.NextRow();

            DumpSpeakerMap(context, alias.speakerMap);
        }
    }
} // namespace

namespace sound_alias_list
{
    void DumperIW3::DumpAsset(AssetDumpingContext& context, const XAssetInfo<AssetSound::Type>& asset)
    {
        const auto* sndAliasList = asset.Asset();

        DumpAliases(context, *sndAliasList);
    }
} // namespace sound_alias_list
