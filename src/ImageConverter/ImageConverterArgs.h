#pragma once

#include "Utils/Arguments/ArgumentParser.h"

#include <cstdint>
#include <string>
#include <vector>

namespace image_converter
{
    enum class Game : std::uint8_t
    {
        UNKNOWN,
        IW3,
        IW4,
        IW5,
        T5,
        T6
    };
} // namespace image_converter

class ImageConverterArgs
{
public:
    ImageConverterArgs();
    bool ParseArgs(int argc, const char** argv, bool& shouldContinue);

    bool m_verbose;
    std::vector<std::string> m_files_to_convert;
    image_converter::Game m_game_to_convert_to;

private:
    /**
     * \brief Prints a command line usage help text for the ImageConverter tool to stdout.
     */
    static void PrintUsage();
    static void PrintVersion();

    void SetVerbose(bool isVerbose);

    ArgumentParser m_argument_parser;
};
