#pragma once
#include <memory>

class ImageConverter
{
public:
    ImageConverter() = default;
    virtual ~ImageConverter() = default;

    ImageConverter(const ImageConverter& other) = delete;
    ImageConverter(ImageConverter&& other) noexcept = delete;
    ImageConverter& operator=(const ImageConverter& other) = delete;
    ImageConverter& operator=(ImageConverter&& other) noexcept = delete;

    /**
     * \brief Starts the ImageConverter application logic.
     * \param argc The amount of command line arguments specified.
     * \param argv The command line arguments.
     * \return \c true if the application was successful or \c false if an error occurred.
     */
    virtual bool Start(int argc, const char** argv) = 0;

    static std::unique_ptr<ImageConverter> Create();
};
