#include "SoundAliasDumperIW3.h"

#include "Csv/CsvStream.h"
#include "Game/IW3/SoundConstantsIW3.h"
#include "Utils/Logging/Log.h"

#include <algorithm>
#include <format>
#include <iomanip>
#include <string>
#include <unordered_set>

using namespace IW3;

namespace
{
    void WriteHeaders(CsvOutputStream& csv)
    {
        for (const auto* header : SOUND_ALIAS_HEADERS)
            csv.WriteColumn(header);
        csv.NextRow();
    }

    void WriteFloat(CsvOutputStream& csv, const float value)
    {
        csv.WriteColumn(std::format("{}", value));
    }

    std::string GetSoundFileName(const SoundFile* soundFile)
    {
        if (!soundFile)
            return {};

        switch (static_cast<snd_alias_type_t>(soundFile->type))
        {
        case SAT_LOADED:
            return soundFile->u.loadSnd && soundFile->u.loadSnd->name ? soundFile->u.loadSnd->name : "";

        case SAT_STREAMED:
        {
            const auto* dir = soundFile->u.streamSnd.dir;
            const auto* name = soundFile->u.streamSnd.name;
            if (!name)
                return {};
            if (!dir || !dir[0])
                return name;

            return std::format("{}/{}", dir, name);
        }

        default:
            return {};
        }
    }

    std::string GetChannelName(const int flags, const char* aliasName)
    {
        const auto channel = static_cast<unsigned>((flags & SND_ALIAS_FLAG_CHANNEL_MASK) >> SND_ALIAS_FLAG_CHANNEL_SHIFT);
        if (channel < std::extent_v<decltype(SOUND_CHANNEL_NAMES)>)
            return SOUND_CHANNEL_NAMES[channel];

        con::warn("Cannot map channel index {} for IW3 sound alias '{}' to the stock channels.def", channel, aliasName ? aliasName : "");
        return {};
    }

    std::string GetSoundTypeName(const snd_alias_t& alias)
    {
        const auto type = alias.soundFile ? static_cast<snd_alias_type_t>(alias.soundFile->type)
                                          : static_cast<snd_alias_type_t>((alias.flags & SND_ALIAS_FLAG_TYPE_MASK) >> SND_ALIAS_FLAG_TYPE_SHIFT);
        const auto typeIndex = static_cast<unsigned>(type);
        return typeIndex < std::extent_v<decltype(SOUND_ALIAS_TYPE_NAMES)> ? SOUND_ALIAS_TYPE_NAMES[typeIndex] : "";
    }

    std::string GetLoopingName(const int flags)
    {
        if ((flags & SND_ALIAS_FLAG_LOOPING) == 0)
            return "nonlooping";

        return flags & SND_ALIAS_FLAG_RANDOM_LOOPING ? "rlooping" : "looping";
    }

    std::string GetMasterSlaveValue(const snd_alias_t& alias)
    {
        if (alias.flags & SND_ALIAS_FLAG_MASTER)
            return "master";
        if (alias.flags & SND_ALIAS_FLAG_SLAVE)
            return std::format("{}", alias.slavePercentage);

        return {};
    }

    std::string GetReverbValue(const int flags)
    {
        std::string value;
        if (flags & SND_ALIAS_FLAG_FULL_DRY_LEVEL)
            value = "fulldrylevel";
        if (flags & SND_ALIAS_FLAG_NO_WET_LEVEL)
        {
            if (!value.empty())
                value += ' ';
            value += "nowetlevel";
        }

        return value;
    }

    float GetSpeakerLevel(const MSSChannelMap& channelMap, const int outputChannel, const int inputChannel)
    {
        const auto speakerCount = std::min(channelMap.speakerCount, static_cast<int>(std::extent_v<decltype(channelMap.speakers)>));
        for (auto speakerIndex = 0; speakerIndex < speakerCount; speakerIndex++)
        {
            const auto& speaker = channelMap.speakers[speakerIndex];
            if (speaker.speaker == outputChannel && inputChannel < speaker.numLevels)
                return speaker.levels[inputChannel];
        }

        return 0.0f;
    }

    void WriteSpeakerMapEntry(std::ostream& stream,
                              const MSSChannelMap& channelMap,
                              const SA_SPKRMAPIDENTIFIERS inputChannel,
                              const SA_SPKRMAPIDENTIFIERS outputChannel)
    {
        stream << SOUND_SPEAKER_MAP_IDENTIFIERS[inputChannel] << ' ' << SOUND_SPEAKER_MAP_IDENTIFIERS[outputChannel] << ' '
               << GetSpeakerLevel(channelMap, SOUND_SPEAKER_MAP_IDENTIFIER_VALUES[outputChannel], SOUND_SPEAKER_MAP_IDENTIFIER_VALUES[inputChannel]) << '\n';
    }

    void DumpSpeakerMap(AssetDumpingContext& context, const SpeakerMap& speakerMap)
    {
        const auto assetFile = context.OpenAssetFile(std::format("soundaliases/{}.spkrmap", speakerMap.name));
        if (!assetFile)
        {
            con::error("Could not create IW3 speaker map '{}'", speakerMap.name);
            return;
        }

        *assetFile << std::fixed << std::setprecision(4);
        *assetFile << "SPKRMAP\n\n";

        const auto& monoToStereo = speakerMap.channelMaps[0][0];
        WriteSpeakerMapEntry(*assetFile, monoToStereo, SA_MONOSOURCE, SA_LEFTSPEAKER);
        WriteSpeakerMapEntry(*assetFile, monoToStereo, SA_MONOSOURCE, SA_RIGHTSPEAKER);
        *assetFile << '\n';

        const auto& stereoToStereo = speakerMap.channelMaps[1][0];
        for (auto outputChannel = 0; outputChannel < 2; outputChannel++)
        {
            const auto outputIdentifier = static_cast<SA_SPKRMAPIDENTIFIERS>(SA_LEFTSPEAKER + outputChannel);
            WriteSpeakerMapEntry(*assetFile, stereoToStereo, SA_LEFTSOURCE, outputIdentifier);
            WriteSpeakerMapEntry(*assetFile, stereoToStereo, SA_RIGHTSOURCE, outputIdentifier);
        }
        *assetFile << '\n';

        const auto& monoToSurround = speakerMap.channelMaps[0][1];
        for (auto outputChannel = 0; outputChannel < 6; outputChannel++)
            WriteSpeakerMapEntry(*assetFile, monoToSurround, SA_MONOSOURCE, static_cast<SA_SPKRMAPIDENTIFIERS>(SA_LEFTSPEAKER + outputChannel));
        *assetFile << '\n';

        const auto& stereoToSurround = speakerMap.channelMaps[1][1];
        for (auto outputChannel = 0; outputChannel < 6; outputChannel++)
        {
            const auto outputIdentifier = static_cast<SA_SPKRMAPIDENTIFIERS>(SA_LEFTSPEAKER + outputChannel);
            WriteSpeakerMapEntry(*assetFile, stereoToSurround, SA_LEFTSOURCE, outputIdentifier);
            WriteSpeakerMapEntry(*assetFile, stereoToSurround, SA_RIGHTSOURCE, outputIdentifier);
        }
    }

    void WriteAlias(CsvOutputStream& csv, const snd_alias_t& alias, const char* fallbackAliasName, const int sequence)
    {
        const auto* aliasName = alias.aliasName && alias.aliasName[0] ? alias.aliasName : fallbackAliasName;

        // Keep this sequence synchronized with SOUND_ALIAS_HEADERS. Each marker identifies
        // the corresponding stock parser field, including fields reordered by the SDK CSV.
        csv.WriteColumn(aliasName ? aliasName : "");             // SA_NAME
        csv.WriteColumn(std::format("{}", sequence));            // SA_SEQUENCE
        csv.WriteColumn(GetSoundFileName(alias.soundFile));      // SA_FILE
        WriteFloat(csv, alias.volMin);                           // SA_VOL_MIN
        WriteFloat(csv, alias.volMax);                           // SA_VOL_MAX
        csv.WriteColumn("");                                     // SA_VOL_MOD: vol_mod is already baked into volMin and volMax.
        WriteFloat(csv, alias.pitchMin);                         // SA_PITCH_MIN
        WriteFloat(csv, alias.pitchMax);                         // SA_PITCH_MAX
        WriteFloat(csv, alias.distMin);                          // SA_DIST_MIN
        WriteFloat(csv, alias.distMax);                          // SA_DIST_MAX
        csv.WriteColumn(GetChannelName(alias.flags, aliasName)); // SA_CHANNEL
        csv.WriteColumn(GetSoundTypeName(alias));                // SA_TYPE
        WriteFloat(csv, alias.probability);                      // SA_PROBABILITY
        csv.WriteColumn(GetLoopingName(alias.flags));            // SA_LOOP
        csv.WriteColumn(GetMasterSlaveValue(alias));             // SA_MASTERSLAVE
        csv.WriteColumn("");                                     // SA_LOADSPEC: source-file filter not stored in the asset.
        csv.WriteColumn(alias.subtitle ? alias.subtitle : "");   // SA_SUBTITLE
        csv.WriteColumn("");                                     // Source-only compression column: applied to the sound file, not stored in the alias.
        csv.WriteColumn(alias.secondaryAliasName ? alias.secondaryAliasName : "");                                                 // SA_SECONDARYALIASNAME
        csv.WriteColumn(alias.volumeFalloffCurve && alias.volumeFalloffCurve->filename ? alias.volumeFalloffCurve->filename : ""); // SA_VOLUMEFALLOFFCURVE
        csv.WriteColumn(std::format("{}", alias.startDelay));                                                                      // SA_STARTDELAY
        csv.WriteColumn(alias.speakerMap && !alias.speakerMap->isDefault && alias.speakerMap->name ? alias.speakerMap->name : ""); // SA_SPEAKERMAP
        csv.WriteColumn(GetReverbValue(alias.flags));                                                                              // SA_REVERB
        WriteFloat(csv, alias.lfePercentage);                                                                                      // SA_LFEPERCENTAGE
        WriteFloat(csv, alias.centerPercentage);                                                                                   // SA_CENTERPERCENTAGE
        csv.WriteColumn("");                                               // Source-only platform filter not stored in the asset.
        WriteFloat(csv, alias.envelopMin);                                 // SA_ENVELOPMIN
        WriteFloat(csv, alias.envelopMax);                                 // SA_ENVELOPMAX
        WriteFloat(csv, alias.envelopPercentage);                          // SA_ENVELOPPERCENTAGE
        csv.WriteColumn(alias.chainAliasName ? alias.chainAliasName : ""); // SA_CHAINALIASNAME
        csv.NextRow();
    }
} // namespace

namespace sound_alias
{
    void DumperIW3::Dump(AssetDumpingContext& context)
    {
        const auto soundAssets = context.m_zone.m_pools.PoolAssets<AssetSound>();
        if (soundAssets.empty())
            return;

        const auto assetFile = context.OpenAssetFile(std::format("soundaliases/{}.csv", context.m_zone.m_name));
        if (!assetFile)
        {
            con::error("Could not create IW3 sound alias CSV for zone '{}'", context.m_zone.m_name);
            context.IncrementProgress();
            return;
        }

        CsvOutputStream csv(*assetFile);
        WriteHeaders(csv);
        std::unordered_set<std::string> dumpedSpeakerMaps;

        for (const auto* assetInfo : soundAssets)
        {
            if (assetInfo->IsReference())
                continue;

            const auto* aliasList = assetInfo->Asset();
            if (!aliasList || aliasList->count <= 0 || !aliasList->head)
                continue;

            for (auto sequence = 0; sequence < aliasList->count; sequence++)
            {
                const auto& alias = aliasList->head[sequence];
                WriteAlias(csv, alias, aliasList->aliasName, sequence);

                const auto* speakerMap = alias.speakerMap;
                if (speakerMap && !speakerMap->isDefault && speakerMap->name && speakerMap->name[0] && dumpedSpeakerMaps.emplace(speakerMap->name).second)
                    DumpSpeakerMap(context, *speakerMap);
            }
        }

        context.IncrementProgress();
    }
} // namespace sound_alias
