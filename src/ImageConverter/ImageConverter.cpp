#include "ImageConverter.h"

#include "ImageConverterArgs.h"

class ImageConverterImpl final : public ImageConverter
{
public:
    bool Start(const int argc, const char** argv) override
    {
        auto shouldContinue = true;
        if (!m_args.ParseArgs(argc, argv, shouldContinue))
            return false;

        if (!shouldContinue)
            return true;

        return true;
    }

private:
    ImageConverterArgs m_args;
};

std::unique_ptr<ImageConverter> ImageConverter::Create()
{
    return std::make_unique<ImageConverterImpl>();
}
