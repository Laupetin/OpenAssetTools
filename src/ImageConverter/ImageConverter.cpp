#include "ImageConverter.h"

#include "Image/DdsLoader.h"
#include "Image/DdsWriter.h"
#include "Image/IwiLoader.h"
#include "Image/IwiWriter13.h"
#include "Image/IwiWriter27.h"
#include "Image/IwiWriter6.h"
#include "Image/IwiWriter8.h"
#include "Image/Texture.h"
#include "ImageConverterArgs.h"
#include "Utils/Logging/Log.h"
#include "Utils/StringUtils.h"

#include <assert.h>
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>

namespace fs = std::filesystem;

namespace image_converter
{
    constexpr auto EXTENSION_IWI = ".iwi";
    constexpr auto EXTENSION_DDS = ".dds";

    class ImageConverterImpl final : public ImageConverter
    {
    public:
        ImageConverterImpl()
            : m_game_to_convert_to(image_converter::Game::UNKNOWN)
        {
        }

        bool Start(const int argc, const char** argv) override
        {
            con::init();

            auto shouldContinue = true;
            if (!m_args.ParseArgs(argc, argv, shouldContinue))
                return false;

            if (!shouldContinue)
                return true;

            m_game_to_convert_to = m_args.m_game_to_convert_to;

            for (const auto& file : m_args.m_files_to_convert)
                Convert(file);

            return true;
        }

    private:
        void Convert(const std::string& file)
        {
            const fs::path filePath(file);
            auto extension = filePath.extension().string();
            utils::MakeStringLowerCase(extension);

            if (extension == EXTENSION_IWI)
                ConvertIwi(filePath);
            else if (extension == EXTENSION_DDS)
                ConvertDds(filePath);
            else
                con::error("Unsupported extension {}", extension);
        }

        bool ConvertIwi(const fs::path& iwiPath)
        {
            std::ifstream file(iwiPath, std::ios::in | std::ios::binary);
            if (!file.is_open())
            {
                con::error("Failed to open input file {}", iwiPath.string());
                return false;
            }

            const auto texture = iwi::LoadIwi(file);
            if (!texture)
                return false;

            auto outPath = iwiPath;
            outPath.replace_extension(".dds");

            std::ofstream outFile(outPath, std::ios::out | std::ios::binary);
            if (!outFile.is_open())
            {
                con::error("Failed to open output file {}", outPath.string());
                return false;
            }

            m_dds_writer.DumpImage(outFile, texture.get());
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

            const auto texture = dds::LoadDds(file);
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

            if (m_game_to_convert_to == Game::UNKNOWN && !ShowGameTui())
                return false;

            switch (m_game_to_convert_to)
            {
            case Game::IW3:
                m_iwi_writer = std::make_unique<iwi6::IwiWriter>();
                break;
            case Game::IW4:
            case Game::IW5:
                m_iwi_writer = std::make_unique<iwi8::IwiWriter>();
                break;
            case Game::T5:
                m_iwi_writer = std::make_unique<iwi13::IwiWriter>();
                break;
            case Game::T6:
                m_iwi_writer = std::make_unique<iwi27::IwiWriter>();
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
                m_game_to_convert_to = Game::IW3;
                break;
            case 2:
                m_game_to_convert_to = Game::IW4;
                break;
            case 3:
                m_game_to_convert_to = Game::IW5;
                break;
            case 4:
                m_game_to_convert_to = Game::T5;
                break;
            case 5:
                m_game_to_convert_to = Game::T6;
                break;
            default:
                con::error("Invalid input");
                return false;
            }

            return true;
        }

        ImageConverterArgs m_args;
        image_converter::Game m_game_to_convert_to;
        DdsWriter m_dds_writer;
        std::unique_ptr<IImageWriter> m_iwi_writer;
    };
} // namespace image_converter

std::unique_ptr<ImageConverter> ImageConverter::Create()
{
    return std::make_unique<image_converter::ImageConverterImpl>();
}
