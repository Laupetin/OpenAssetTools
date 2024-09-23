#pragma once

#include "Utils/Arguments/ArgumentParser.h"
#include "Utils/ClassUtils.h"

#include <string>
#include <vector>

class ImageConverterArgs
{
public:
    ImageConverterArgs();
    bool ParseArgs(int argc, const char** argv, bool& shouldContinue);

    bool m_verbose;
    std::vector<std::string> m_files_to_convert;

private:
    /**
     * \brief Prints a command line usage help text for the ImageConverter tool to stdout.
     */
    static void PrintUsage();
    static void PrintVersion();

    void SetVerbose(bool isVerbose);

    ArgumentParser m_argument_parser;
};
