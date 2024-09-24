#include "ImageConverter.h"

#include "Image/Texture.h"
#include "ImageConverterArgs.h"
#include "Utils/StringUtils.h"

#include <filesystem>

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
            fs::path filePath(file);
            auto extension = filePath.extension().string();
            utils::MakeStringLowerCase(extension);

            if (extension == EXTENSION_IWI)
            {
            }
        }

        ImageConverterArgs m_args;
        image_converter::Game m_game_to_convert_to;
    };
} // namespace image_converter

std::unique_ptr<ImageConverter> ImageConverter::Create()
{
    return std::make_unique<image_converter::ImageConverterImpl>();
}
