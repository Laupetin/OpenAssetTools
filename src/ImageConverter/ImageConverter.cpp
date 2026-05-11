#include "ImageConverter.h"

#include "Game/IGame.h"
#include "Image/DdsLoader.h"
#include "Image/DdsWriter.h"
#include "Image/IwiLoader.h"
#include "Image/IwiWriter13.h"
#include "Image/IwiWriter27.h"
#include "Image/IwiWriter6.h"
#include "Image/IwiWriter8.h"
#include "ImageConverterArgs.h"
#include "ObjContainer/IPak/IPak.h"
#include "Utils/Logging/Log.h"
#include "Utils/StringUtils.h"

#include <assert.h>
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <optional>

namespace fs = std::filesystem;
using namespace image;

namespace
{
    constexpr auto EXTENSION_IWI = ".iwi";
    constexpr auto EXTENSION_DDS = ".dds";
    constexpr auto EXTENSION_IPAK = ".ipak";

    class ImageConverter
    {
    public:
        explicit ImageConverter(const ImageConverterArgs& args)
            : m_game_to_convert_to(args.m_game_to_convert_to)
        {
        }

        void HandleFile(const std::string& file)
        {
            const fs::path filePath(file);
            auto extension = filePath.extension().string();
            utils::MakeStringLowerCase(extension);

            if (extension == EXTENSION_IWI)
                ConvertIwi(filePath);
            else if (extension == EXTENSION_DDS)
                ConvertDds(filePath);
            else if (extension == EXTENSION_IPAK)
                ExtractIpak(filePath);
            else
                con::error("Unsupported extension {}", extension);
        }

    private:
        bool ConvertIwi(const fs::path& iwiPath)
        {
            std::ifstream file(iwiPath, std::ios::in | std::ios::binary);
            if (!file.is_open())
            {
                con::error("Failed to open input file {}", iwiPath.string());
                return false;
            }

            const auto loadResult = image::LoadIwi(file);
            if (!loadResult)
                return false;

            auto outPath = iwiPath;
            outPath.replace_extension(".dds");

            std::ofstream outFile(outPath, std::ios::out | std::ios::binary);
            if (!outFile.is_open())
            {
                con::error("Failed to open output file {}", outPath.string());
                return false;
            }

            m_dds_writer.DumpImage(outFile, loadResult->m_texture.get());
            return true;
        }

        bool ConvertDds(const fs::path& ddsPath)
        {
            std::ifstream file(ddsPath, std::ios::in | std::ios::binary);
            if (!file.is_open())
            {
                con::error("Failed to open input file {}", ddsPath.string());
                return false;
            }

            const auto texture = image::LoadDds(file);
            if (!texture)
                return false;

            if (!EnsureIwiWriterIsPresent())
                return false;

            auto outPath = ddsPath;
            outPath.replace_extension(".iwi");

            std::ofstream outFile(outPath, std::ios::out | std::ios::binary);
            if (!outFile.is_open())
            {
                con::error("Failed to open output file {}", outPath.string());
                return false;
            }

            m_iwi_writer->DumpImage(outFile, texture.get());
            return true;
        }

        bool EnsureIwiWriterIsPresent()
        {
            if (m_iwi_writer)
                return true;

            if (!m_game_to_convert_to && !ShowGameTui())
                return false;

            switch (*m_game_to_convert_to)
            {
            case GameId::IW3:
                m_iwi_writer = std::make_unique<image::iwi6::IwiWriter>();
                break;
            case GameId::IW4:
            case GameId::IW5:
                m_iwi_writer = std::make_unique<image::iwi8::IwiWriter>();
                break;
            case GameId::T5:
                m_iwi_writer = std::make_unique<image::iwi13::IwiWriter>();
                break;
            case GameId::T6:
                m_iwi_writer = std::make_unique<image::iwi27::IwiWriter>();
                break;
            default:
                assert(false);
                return false;
            }

            return true;
        }

        bool ShowGameTui()
        {
            con::info("Select the game to convert to:");
            con::info("  1 - Call Of Duty 4: Modern Warfare (IW3)");
            con::info("  2 - Call Of Duty: Modern Warfare 2 (IW4)");
            con::info("  3 - Call Of Duty: Modern Warfare 3 (IW5)");
            con::info("  4 - Call Of Duty: Black Ops (T5)");
            con::info("  5 - Call Of Duty: Black Ops 2 (T6)");

            unsigned num;
            std::cin >> num;

            switch (num)
            {
            case 1:
                m_game_to_convert_to = GameId::IW3;
                break;
            case 2:
                m_game_to_convert_to = GameId::IW4;
                break;
            case 3:
                m_game_to_convert_to = GameId::IW5;
                break;
            case 4:
                m_game_to_convert_to = GameId::T5;
                break;
            case 5:
                m_game_to_convert_to = GameId::T6;
                break;
            default:
                con::error("Invalid input");
                return false;
            }

            return true;
        }

        bool ExtractIpak(const fs::path& ipakPath)
        {
            auto file = std::make_unique<std::ifstream>(ipakPath, std::ios::in | std::ios::binary);
            if (!file->is_open())
            {
                con::error("Failed to open ipak {}", ipakPath.string());
                return false;
            }

            auto ipak = IIPak::Create(ipakPath.string(), std::move(file));
            if (!ipak->Initialize())
            {
                con::error("Failed to read ipak {}", ipakPath.string());
                return false;
            }

            const auto outDir = fs::absolute(ipakPath).parent_path() / ipakPath.filename().replace_extension();
            fs::create_directories(outDir);

            for (const auto& indexEntry : ipak->GetIndexEntries())
            {
                const auto fileName = std::format("{:6x}_{:6x}.iwi", indexEntry.key.dataHash, indexEntry.key.nameHash);
                std::ofstream outFile(outDir / fileName, std::ios::out | std::ios::binary);
                if (!outFile.is_open())
                {
                    con::error("Failed to open ipak file {}", fileName);
                    return false;
                }

                auto entryStream = ipak->GetEntryStream(indexEntry.key.nameHash, indexEntry.key.dataHash);
                if (!entryStream)
                {
                    con::error("Failed to open entry stream for {}", fileName);
                    return false;
                }

                char buffer[0x2000];
                entryStream->read(buffer, 0x2000);
                auto readCount = entryStream->gcount();
                while (readCount > 0)
                {
                    outFile.write(buffer, readCount);

                    entryStream->read(buffer, 0x2000);
                    readCount = entryStream->gcount();
                }

                entryStream->close();
                outFile.close();
            }

            return true;
        }

        ImageConverterArgs m_args;
        std::optional<GameId> m_game_to_convert_to;
        DdsWriter m_dds_writer;
        std::unique_ptr<ImageWriter> m_iwi_writer;
    };
} // namespace

bool RunImageConverter(const int argc, const char** argv)
{
    con::init();

    auto shouldContinue = true;
    ImageConverterArgs args;
    if (!args.ParseArgs(argc, argv, shouldContinue))
        return false;

    if (!shouldContinue)
        return true;

    ImageConverter imageConverter(args);
    for (const auto& file : args.m_files_to_convert)
        imageConverter.HandleFile(file);

    return true;
}
