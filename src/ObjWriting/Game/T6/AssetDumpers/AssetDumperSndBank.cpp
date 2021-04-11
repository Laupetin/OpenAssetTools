#include "AssetDumperSndBank.h"

#include <filesystem>
#include <unordered_map>

#include "Csv/CsvStream.h"
#include "ObjContainer/SoundBank/SoundBank.h"

using namespace T6;
namespace fs = std::filesystem;

std::string AssetDumperSndBank::GetExtensionForFormat(const snd_asset_format format)
{
    switch(format)
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

std::unique_ptr<std::ostream> AssetDumperSndBank::OpenAssetOutputFile(AssetDumpingContext& context, const std::string& outputFileName, const SoundAssetBankEntry& entry) const
{
    fs::path assetPath(context.m_base_path);

    fs::path assetName(outputFileName);
    auto firstPart = true;
    for(const auto& part : assetName)
    {
        if(firstPart)
        {
            firstPart = false;
            if(part.string() == "raw"
                || part.string() == "devraw")
                continue;
        }

        assetPath.append(part.string());
    }

    const auto extension = GetExtensionForFormat(static_cast<snd_asset_format>(entry.format));
    if(!extension.empty())
        assetPath.concat(extension);

    auto assetDir(assetPath);
    assetDir.remove_filename();

    create_directories(assetDir);

    auto outputStream = std::make_unique<std::ofstream>(assetPath, std::ios_base::out | std::ios_base::binary);

    if(outputStream->is_open())
    {
        return std::move(outputStream);
    }

    return nullptr;
}

std::unique_ptr<std::ostream> AssetDumperSndBank::OpenAliasOutputFile(AssetDumpingContext& context, SndBank* sndBank) const
{
    return nullptr;
}

void AssetDumperSndBank::DumpSndBankAliases(AssetDumpingContext& context, SndBank* sndBank, std::unordered_map<unsigned, std::string>& aliasFiles)
{
    for (auto i = 0u; i < sndBank->aliasCount; i++)
    {
        const auto& aliasList = sndBank->alias[i];
        for (auto j = 0; j < aliasList.count; j++)
        {
            const auto& alias = aliasList.head[j];
            if (alias.assetId && alias.assetFileName)
                aliasFiles[alias.assetId] = alias.assetFileName;
        }
    }
}

void AssetDumperSndBank::DumpSndBank(AssetDumpingContext& context, const XAssetInfo<SndBank>* sndBankInfo)
{
    const auto* sndBank = sndBankInfo->Asset();

    std::unordered_map<unsigned int, std::string> aliasMappings;

    for (const auto& [id, filename] : aliasMappings)
    {
        auto foundEntry = false;

        for (const auto* soundBank : SoundBank::Repository)
        {
            auto soundFile = soundBank->GetEntryStream(id);
            if (soundFile.IsOpen())
            {
                auto outFile = OpenAssetOutputFile(context, filename, soundFile.m_entry);
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

        if(!foundEntry)
        {
            std::cout << "Could not find data for sound \"" << filename << "\"" << std::endl;
        }
    }
}

void AssetDumperSndBank::DumpPool(AssetDumpingContext& context, AssetPool<SndBank>* pool)
{
    for(const auto* assetInfo : *pool)
    {
        if (!assetInfo->m_name.empty() && assetInfo->m_name[0] == ',')
            continue;

        DumpSndBank(context, assetInfo);
    }
}
