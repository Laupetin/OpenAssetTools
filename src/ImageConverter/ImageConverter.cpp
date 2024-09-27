#include "ImageConverter.h"

#include "Image/DdsWriter.h"
#include "Image/IwiLoader.h"
#include "Image/Texture.h"
#include "ImageConverterArgs.h"
#include "Utils/StringUtils.h"

#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>

namespace fs = std::filesystem;

namespace image_converter
{
    constexpr auto EXTENSION_IWI = ".iwi";

    class ImageLoader
    {
    public:
        ImageLoader() = default;
        virtual ~ImageLoader() = default;
        ImageLoader(const ImageLoader& other) = default;
        ImageLoader(ImageLoader&& other) noexcept = default;
        ImageLoader& operator=(const ImageLoader& other) = default;
        ImageLoader& operator=(ImageLoader&& other) noexcept = default;

        // virtual Texture*
    };

    class ImageConverterImpl final : public ImageConverter
    {
    public:
        ImageConverterImpl()
            : m_game_to_convert_to(image_converter::Game::UNKNOWN)
        {
        }

        bool Start(const int argc, const char** argv) override
        {
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
            {
                ConvertIwi(filePath);
            }
            else
            {
                std::cerr << std::format("Unsupported extension {}\n", extension);
            }
        }

        bool ConvertIwi(const fs::path& iwiPath)
        {
            std::ifstream file(iwiPath, std::ios::in | std::ios::binary);
            if (!file.is_open())
            {
                std::cerr << std::format("Failed to open input file {}\n", iwiPath.string());
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
                std::cerr << std::format("Failed to open output file {}\n", outPath.string());
                return false;
            }

            m_dds_writer.DumpImage(outFile, texture.get());
            return true;
        }

        ImageConverterArgs m_args;
        image_converter::Game m_game_to_convert_to;
        DdsWriter m_dds_writer;
    };
} // namespace image_converter

std::unique_ptr<ImageConverter> ImageConverter::Create()
{
    return std::make_unique<image_converter::ImageConverterImpl>();
}
