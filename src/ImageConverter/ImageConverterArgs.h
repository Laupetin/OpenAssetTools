#pragma once

#include "Game/IGame.h"
#include "Utils/Arguments/ArgumentParser.h"

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

class ImageConverterArgs
{
public:
    ImageConverterArgs();
    bool ParseArgs(int argc, const char** argv, bool& shouldContinue);

    std::vector<std::string> m_files_to_convert;
    std::optional<GameId> m_game_to_convert_to;

private:
    /**
     * \brief Prints a command line usage help text for the ImageConverter tool to stdout.
     */
    static void PrintUsage();
    static void PrintVersion();

    ArgumentParser m_argument_parser;
};
