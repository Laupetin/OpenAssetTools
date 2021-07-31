#include "AssetDumperSndBank.h"

#include <fstream>
#include <filesystem>
#include <unordered_map>

#include "Utils/ClassUtils.h"
#include "Csv/CsvStream.h"
#include "ObjContainer/SoundBank/SoundBank.h"

using namespace T6;
namespace fs = std::filesystem;

class AssetDumperSndBank::Internal
{
    inline static const std::string ALIAS_HEADERS[]
    {
        "# name",
        "# file",
        "# template",
        "# loadspec",
        "# secondary",
        "# group",
        "# vol_min",
        "# vol_max",
        "# team_vol_mod",
        "# dist_min",
        "# dist_max",
        "# dist_reverb_max",
        "# volume_falloff_curve",
        "# reverb_falloff_curve",
        "# volume_min_falloff_curve",
        "# reverb_min_falloff_curve",
        "# limit_count",
        "# limit_type",
        "# entity_limit_count",
        "# entity_limit_type",
        "# pitch_min",
        "# pitch_max",
        "# team_pitch_mod",
        "# min_priority",
        "# max_priority",
        "# min_priority_threshold",
        "# max_priority_threshold",
        "# spatialized",
        "# type",
        "# loop",
        "# randomize_type",
        "# probability",
        "# start_delay",
        "# reverb_send",
        "# duck",
        "# pan",
        "# center_send",
        "# envelop_min",
        "# envelop_max",
        "# envelop_percentage",
        "# occlusion_level",
        "# occlusion_wet_dry",
        "# is_big",
        "# distance_lpf",
        "# move_type",
        "# move_time",
        "# real_delay",
        "# subtitle",
        "# mature",
        "# doppler",
        "# futz",
        "# context_type",
        "# context_value",
        "# compression",
        "# timescale",
        "# music",
        "# fade_in",
        "# fade_out",
        "# pc_format",
        "# pause",
        "# stop_on_death",
        "# bus",
        "# snapshot",
    };

    AssetDumpingContext& m_context;

    static std::string GetExtensionForFormat(const snd_asset_format format)
    {
        switch (format)
        {
        case SND_ASSET_FORMAT_MP3:
            return ".mp3";

        case SND_ASSET_FORMAT_FLAC:
            return ".flac";

        case SND_ASSET_FORMAT_PCMS16:
        case SND_ASSET_FORMAT_PCMS24:
        case SND_ASSET_FORMAT_PCMS32:
        case SND_ASSET_FORMAT_IEEE:
        case SND_ASSET_FORMAT_XMA4:
        case SND_ASSET_FORMAT_MSADPCM:
        case SND_ASSET_FORMAT_WMA:
        case SND_ASSET_FORMAT_WIIUADPCM:
        case SND_ASSET_FORMAT_MPC:
            std::cout << "Unsupported sound format " << format << std::endl;
            return std::string();

        default:
            assert(false);
            std::cout << "Unknown sound format " << format << std::endl;
            return std::string();
        }
    }

    _NODISCARD std::string GetAssetFilename(const std::string& outputFileName, const SoundAssetBankEntry& entry) const
    {
        fs::path assetPath(m_context.m_base_path);

        fs::path assetName(outputFileName);
        auto firstPart = true;
        for (const auto& part : assetName)
        {
            if (firstPart)
            {
                firstPart = false;
                if (part.string() == "raw"
                    || part.string() == "devraw")
                    continue;
            }

            assetPath.append(part.string());
        }

        const auto extension = GetExtensionForFormat(static_cast<snd_asset_format>(entry.format));
        if (!extension.empty())
            assetPath.concat(extension);

        return assetPath.string();
    }

    _NODISCARD std::unique_ptr<std::ostream> OpenAssetOutputFile(const std::string& outputFileName, const SoundAssetBankEntry& entry) const
    {
        fs::path assetPath(GetAssetFilename(outputFileName, entry));

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

    std::unique_ptr<std::ostream> OpenAliasOutputFile(const SndBank* sndBank) const
    {
        return nullptr;
    }

    static void WriteAliasFileHeader(CsvOutputStream& stream)
    {
        for (const auto& headerField : ALIAS_HEADERS)
        {
            stream.WriteColumn(headerField);
        }

        stream.NextRow();
    }

    void WriteAliasToFile(CsvOutputStream& stream, const SndAlias* alias)
    {
        // name
        stream.WriteColumn(alias->name);

        // file
        stream.WriteColumn(alias->assetFileName);

        // "# template",
        // template
        stream.WriteColumn("");

        // loadspec
        stream.WriteColumn("");
        
        //     "# secondary",
        //     "# group",
        //     "# vol_min",
        //     "# vol_max",
        //     "# team_vol_mod",
        //     "# dist_min",
        //     "# dist_max",
        //     "# dist_reverb_max",
        //     "# volume_falloff_curve",
        //     "# reverb_falloff_curve",
        //     "# volume_min_falloff_curve",
        //     "# reverb_min_falloff_curve",
        //     "# limit_count",
        //     "# limit_type",
        //     "# entity_limit_count",
        //     "# entity_limit_type",
        //     "# pitch_min",
        //     "# pitch_max",
        //     "# team_pitch_mod",
        //     "# min_priority",
        //     "# max_priority",
        //     "# min_priority_threshold",
        //     "# max_priority_threshold",
        //     "# spatialized",
        //     "# type",
        //     "# loop",
        //     "# randomize_type",
        //     "# probability",
        //     "# start_delay",
        //     "# reverb_send",
        //     "# duck",
        //     "# pan",
        //     "# center_send",
        //     "# envelop_min",
        //     "# envelop_max",
        //     "# envelop_percentage",
        //     "# occlusion_level",
        //     "# occlusion_wet_dry",
        //     "# is_big",
        //     "# distance_lpf",
        //     "# move_type",
        //     "# move_time",
        //     "# real_delay",
        //     "# subtitle",
        //     "# mature",
        //     "# doppler",
        //     "# futz",
        //     "# context_type",
        //     "# context_value",
        //     "# compression",
        //     "# timescale",
        //     "# music",
        //     "# fade_in",
        //     "# fade_out",
        //     "# pc_format",
        //     "# pause",
        //     "# stop_on_death",
        //     "# bus",
        //     "# snapshot",
    }

    void DumpSndBankAliases(const SndBank* sndBank, std::unordered_map<unsigned, std::string>& aliasFiles)
    {
        const auto outputFile = OpenAliasOutputFile(sndBank);

        if (outputFile == nullptr)
        {
            std::cout << "Failed to open sound alias output file for: \"" << sndBank->name << "\"" << std::endl;
            return;
        }

        CsvOutputStream csvStream(*outputFile);
        WriteAliasFileHeader(csvStream);

        for (auto i = 0u; i < sndBank->aliasCount; i++)
        {
            const auto& aliasList = sndBank->alias[i];
            for (auto j = 0; j < aliasList.count; j++)
            {
                const auto& alias = aliasList.head[j];
                WriteAliasToFile(csvStream, &alias);
                if (alias.assetId && alias.assetFileName)
                    aliasFiles[alias.assetId] = alias.assetFileName;
            }
        }
    }

    void DumpSoundData(std::unordered_map<unsigned, std::string>& aliasFiles) const
    {
        for (const auto& [id, filename] : aliasFiles)
        {
            auto foundEntry = false;

            for (const auto* soundBank : SoundBank::Repository)
            {
                auto soundFile = soundBank->GetEntryStream(id);
                if (soundFile.IsOpen())
                {
                    auto outFile = OpenAssetOutputFile(filename, soundFile.m_entry);
                    if (!outFile)
                    {
                        std::cout << "Failed to open sound outputfile: \"" << filename << "\"" << std::endl;
                        break;
                    }

                    while (!soundFile.m_stream->eof())
                    {
                        char buffer[2048];
                        soundFile.m_stream->read(buffer, sizeof(buffer));
                        const auto readSize = soundFile.m_stream->gcount();
                        outFile->write(buffer, readSize);
                    }

                    foundEntry = true;
                    break;
                }
            }

            if (!foundEntry)
            {
                std::cout << "Could not find data for sound \"" << filename << "\"" << std::endl;
            }
        }
    }

    void DumpSndBank(const XAssetInfo<SndBank>* sndBankInfo)
    {
        const auto* sndBank = sndBankInfo->Asset();

        std::unordered_map<unsigned int, std::string> aliasMappings;
        DumpSndBankAliases(sndBank, aliasMappings);
        DumpSoundData(aliasMappings);
    }

public:
    explicit Internal(AssetDumpingContext& context)
        : m_context(context)
    {
    }

    void DumpPool(AssetPool<SndBank>* pool)
    {
        for (const auto* assetInfo : *pool)
        {
            if (!assetInfo->m_name.empty() && assetInfo->m_name[0] == ',')
                continue;

            DumpSndBank(assetInfo);
        }
    }
};

void AssetDumperSndBank::DumpPool(AssetDumpingContext& context, AssetPool<SndBank>* pool)
{
    Internal internal(context);
    internal.DumpPool(pool);
}
