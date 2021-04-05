#include "AssetDumperSndBank.h"

#include <filesystem>

#include "Csv/CsvStream.h"
#include "ObjContainer/SoundBank/SoundBank.h"

using namespace T6;
namespace fs = std::filesystem;

std::unique_ptr<std::ostream> AssetDumperSndBank::OpenOutputFile(AssetDumpingContext& context, const std::string& outputFileName) const
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

void AssetDumperSndBank::DumpSndBank(AssetDumpingContext& context, const XAssetInfo<SndBank>* sndBankInfo)
{
    const auto* sndBank = sndBankInfo->Asset();

    std::map<unsigned int, std::string> aliasMappings;
    for (auto i = 0u; i < sndBank->aliasCount; i++)
    {
        const auto& aliasList = sndBank->alias[i];
        for (auto j = 0; j < aliasList.count; j++)
        {
            const auto& alias = aliasList.head[j];
            if (alias.assetId && alias.assetFileName)
                aliasMappings[alias.assetId] = alias.assetFileName;
        }
    }

    for (const auto& [id, filename] : aliasMappings)
    {
        auto foundEntry = false;

        for (const auto* soundBank : SoundBank::Repository)
        {
            auto soundFile = soundBank->GetEntryStream(id);
            if (soundFile.IsOpen())
            {
                auto outFile = OpenOutputFile(context, filename);
                if (!outFile)
                {
                    std::cout << "Failed to open sound outputfile: \"" << filename << "\"" << std::endl;
                    break;
                }

                char buffer[2048];
                while (!soundFile.m_stream->eof())
                {
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
