#include "SndAliasListDumperIW3.h"

#include "Csv/CsvStream.h"
#include "Sound/WavWriter.h"
#include "Utils/Logging/Log.h"

#include <Game/IW3/Sound/SndAliasListFields.h>
#include <algorithm>
#include <filesystem>
#include <format>
#include <fstream>
#include <sstream>
#include <cassert>

using namespace IW3;
namespace fs = std::filesystem;

namespace
{
    const std::string ALIAS_HEADERS[]{"name",
                                      "sequence",
                                      "file",
                                      "vol_min",
                                      "vol_max",
                                      "vol_mod",
                                      "pitch_min",
                                      "pitch_max",
                                      "dist_min",
                                      "dist_max",
                                      "channel",
                                      "type",
                                      "probability",
                                      "loop",
                                      "masterslave",
                                      "loadspec",
                                      "subtitle",
                                      "compression",
                                      "secondaryaliasname",
                                      "volumefalloffcurve",
                                      "startdelay",
                                      "speakermap",
                                      "reverb",
                                      "lfe percentage",
                                      "center percentage",
                                      "platform",
                                      "envelop_min",
                                      "envelop_max",
                                      "envelop percentage",
                                      "conversion"};

    const std::string PREFIXES_TO_DROP[]{
        "raw/",
        "devraw/",
    };

    [[nodiscard]] std::string GetAssetFilename(const AssetDumpingContext& context, std::string outputFileName, const std::string& extension)
    {
        fs::path assetPath(context.m_base_path);

        std::ranges::replace(outputFileName, '\\', '/');
        for (const auto& droppedPrefix : PREFIXES_TO_DROP)
        {
            if (outputFileName.rfind(droppedPrefix, 0) != std::string::npos)
            {
                outputFileName.erase(0, droppedPrefix.size());
                break;
            }
        }

        assetPath.append(outputFileName);
        if (!extension.empty())
            assetPath.concat(extension);

        return assetPath.string();
    }

    std::unique_ptr<std::ostream> OpenAssetOutputFile(const AssetDumpingContext& context, const std::string& outputFileName, const std::string& extension)
    {
        fs::path assetPath(GetAssetFilename(context, outputFileName, extension));

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

    void WriteAliasFileHeader(CsvOutputStream& stream)
    {
        for (const auto& headerField : ALIAS_HEADERS)
        {
            stream.WriteColumn(headerField);
        }

        stream.NextRow();
    }

    void WriteColumnString(CsvOutputStream& stream, const std::string& stringValue)
    {
        stream.WriteColumn(stringValue);
    }

    void WriteColumnString(CsvOutputStream& stream, const char* stringValue)
    {
        stream.WriteColumn(stringValue ? std::string(stringValue) : std::string());
    }

    template<typename T> void WriteColumnIntegral(CsvOutputStream& stream, const T value, const T defaultVal = T())
    {
        if (value == defaultVal)
        {
            stream.WriteColumn("");
        }
        else
        {
            stream.WriteColumn(std::format("{}", value));
        }
    }

    void WriteColumnEnumWithSize(CsvOutputStream& stream, const unsigned value, const char* const* enumValues, const size_t enumValueCount)
    {
        assert(value < enumValueCount);
        stream.WriteColumn(value < enumValueCount ? enumValues[value] : "");
    }

    template<size_t Size> void WriteColumnEnum(CsvOutputStream& stream, const unsigned value, const char* const (&enumValues)[Size])
    {
        WriteColumnEnumWithSize(stream, value, enumValues, Size);
    }

    void WriteColumnEnumFlagsWithSize(CsvOutputStream& stream, const unsigned value, const char* const* enumValues, const size_t enumValueCount)
    {
        assert(enumValueCount <= 32u);
        std::ostringstream ss;
        auto first = true;
        for (auto i = 0u; i < enumValueCount; i++)
        {
            const auto flagValue = 1u << i;
            if (value & flagValue)
            {
                if (first)
                    first = false;
                else
                    ss << ' ';

                ss << enumValues[i];
            }
        }

        stream.WriteColumn(ss.str());
    }

    template<size_t Size> void WriteColumnEnumFlags(CsvOutputStream& stream, const unsigned value, const char* const (&enumValues)[Size])
    {
        WriteColumnEnumFlagsWithSize(stream, value, enumValues, Size);
    }

    void WriteColumnVolumeLinear(CsvOutputStream& stream, float value)
    {
        stream.WriteColumn(std::format("{:.2f}", value));
    }

    void WriteColumnFloat(CsvOutputStream& stream, const float& value, const float& defaultVal = -1.0f, int precision = 2)
    {
        int intVal = static_cast<int>(value);
        if (value == defaultVal)
        {
            stream.WriteColumn("");
        }
        else if (value == intVal)
        {
            stream.WriteColumn(std::format("{}", intVal));
        }
        else
        {
            stream.WriteColumn(std::format("{:.{}g}", value, precision));
        }
    }

    void WriteChannelEnum(CsvOutputStream& stream, int channel)
    {
        assert(channel < std::extent_v<decltype(snd_alias_channel_names)>);

        stream.WriteColumn(snd_alias_channel_names[channel]);
    }

    const char* FromReferencedString(const char* refString)
    {
        if (refString[0] == ',')
        {
            return (refString + 1);
        }
        return refString;
    }

    void WriteAliasToFile(CsvOutputStream& stream, const snd_alias_t& alias, int sequence = 0)
    {
        // name
        WriteColumnString(stream, alias.aliasName);

        // sequence
        WriteColumnIntegral(stream, (alias.sequence ? alias.sequence : sequence), 0);

        // file
        if (alias.soundFile)
        {
            if (alias.soundFile->type == SAT_STREAMED && alias.soundFile->u.streamSnd.dir && alias.soundFile->u.streamSnd.name)
            {
                WriteColumnString(stream, std::format("{}/{}", alias.soundFile->u.streamSnd.dir, alias.soundFile->u.streamSnd.name));
            }
            else if (alias.soundFile->type == SAT_LOADED && alias.soundFile->u.loadSnd->name)
            {
                WriteColumnString(stream, FromReferencedString(alias.soundFile->u.loadSnd->name));
            }
        }

        // vol_min
        WriteColumnVolumeLinear(stream, alias.volMin);

        // vol_max
        WriteColumnVolumeLinear(stream, alias.volMax);

        // vol_mod
        // match a string in the volumemodgroups.def file
        WriteColumnString(stream, "max");

        // pitch_min
        WriteColumnFloat(stream, alias.pitchMin, 1.0f);

        // pitch_max
        WriteColumnFloat(stream, alias.pitchMax, 1.0f);

        // dist_min
        WriteColumnFloat(stream, alias.distMin, 120.0f);

        // dist_max
        WriteColumnFloat(stream, alias.distMax, 500000.0f);

        // channel
        WriteChannelEnum(stream, (alias.flags >> 8) & 0x3F);

        // type
        if ((alias.flags & 0x40) && (alias.flags & 0x80))
        {
            // Not sure if both flags set means primed
            WriteColumnString(stream, "primed");
        }
        else if (alias.flags & 0x80 && !(alias.flags & 0x40))
        {
            WriteColumnString(stream, "streamed");
        }

        // probability
        WriteColumnFloat(stream, alias.probability, 1.0f);

        // loop
        if (alias.flags & 0x1)
        {
            if (alias.flags & 0x20)
            {
                WriteColumnString(stream, "rlooping");
            }
            else
            {
                WriteColumnString(stream, "looping");
            }
        }
        else
        {
            WriteColumnString(stream, "");
        }

        // masterslave
        if (alias.flags & 0x2)
        {
            WriteColumnString(stream, "master");
        }
        else
        {
            WriteColumnFloat(stream, alias.slavePercentage, 1.0f);
        }

        // loadspec
        WriteColumnString(stream, "");

        // subtitle
        if (alias.subtitle)
        {
            WriteColumnString(stream, alias.subtitle);
        }
        else
        {
            WriteColumnString(stream, "");
        }

        // compression
        WriteColumnString(stream, "");

        // secondaryaliasname
        if (alias.secondaryAliasName)
        {
            WriteColumnString(stream, alias.secondaryAliasName);
        }
        else
        {
            WriteColumnString(stream, "");
        }

        // volumefalloffcurve
        if (alias.volumeFalloffCurve && alias.volumeFalloffCurve->filename)
        {
            WriteColumnString(stream, alias.volumeFalloffCurve->filename);
        }
        else
        {
            WriteColumnString(stream, "");
        }

        // startdelay
        WriteColumnIntegral(stream, alias.startDelay);

        // speakermap
        if (alias.speakerMap && alias.speakerMap->name)
        {
            WriteColumnString(stream, alias.speakerMap->name);
        }
        else
        {
            WriteColumnString(stream, "");
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
        WriteColumnString(stream, reverbString);

        // lfe percentage
        WriteColumnFloat(stream, alias.lfePercentage, 0);

        // center percentage,
        WriteColumnFloat(stream, alias.centerPercentage, 0);

        // platform

        // envelop_min
        WriteColumnFloat(stream, alias.envelopMin, 0);

        // envelop_max
        WriteColumnFloat(stream, alias.envelopMax, 0);

        // envelop percentage
        WriteColumnFloat(stream, alias.envelopPercentage, 0);

        // conversion
        WriteColumnString(stream, "");
    }

    const char* SourceName(int numLevels, int levelIndex)
    {
        if (numLevels == 1)
            return "MONOSOURCE";
        return (levelIndex == 0) ? "LEFTSOURCE" : "RIGHTSOURCE";
    }

    void DumpSpeakerMap(const AssetDumpingContext& context, const SpeakerMap* speakerMap)
    {
        if (!speakerMap || !speakerMap->name || speakerMap->name[0] == '\0')
            return;

        const auto outFile = OpenAssetOutputFile(context, std::format("soundaliases/{}", speakerMap->name), ".spkrmap");
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
                        stream << SourceName(speakers.numLevels, l) << "\t\t" << speaker_map_names[speakers.speaker] << '\t' << std::fixed
                               << std::setprecision(4) << speakers.levels[l] << "\r\n";
                    }
                }
            }
        }
    }

    void DumpSoundFilePcm(const AssetDumpingContext& context, const char* assetFileName, const MssSound& soundFile)
    {
        const char* cleanedAssetFileName = assetFileName;
        if (assetFileName[0] == ',')
        {
            cleanedAssetFileName = (assetFileName + 1);
        }

        const auto outFile = OpenAssetOutputFile(context, std::format("sound/{}", cleanedAssetFileName), "");
        if (!outFile)
        {
            con::error("Failed to open sound output file: \"{}\"", cleanedAssetFileName);
            return;
        }

        const WavWriter writer(*outFile);

        AILSOUNDINFO info = soundFile.info;
        const WavMetaData metaData{info.channels, info.rate, info.bits};

        writer.WritePcmHeader(metaData, info.data_len);
        outFile->write(soundFile.data, info.data_len);
    }

    void DumpAliases(const AssetDumpingContext& context, const snd_alias_list_t& sndAliasList)
    {
        const auto outFile = OpenAssetOutputFile(context, std::format("soundaliases/{}", sndAliasList.aliasName), ".csv");
        if (!outFile)
        {
            con::error("Failed to open sound alias list output file: \"\"", sndAliasList.aliasName);
            return;
        }

        CsvOutputStream csvStream(*outFile);
        WriteAliasFileHeader(csvStream);

        for (auto i = 0u; i < sndAliasList.count; i++)
        {
            const auto& alias = sndAliasList.head[i];

            if (sndAliasList.count == 1)
            {
                WriteAliasToFile(csvStream, alias);
            }
            else
            {
                WriteAliasToFile(csvStream, alias, i + 1);
            }

            csvStream.NextRow();

            DumpSpeakerMap(context, alias.speakerMap);

            if (alias.soundFile->type == SAT_LOADED)
            {
                auto loadSnd = alias.soundFile->u.loadSnd;
                if (loadSnd && loadSnd->name)
                {
                    DumpSoundFilePcm(context, loadSnd->name, loadSnd->sound);
                }
            }
            else if (alias.soundFile->type != SAT_STREAMED)
            {
                con::warn("Unhandled sound type: {}", alias.soundFile->type);
            }
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
