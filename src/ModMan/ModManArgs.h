#pragma once

#include "Utils/Arguments/ArgumentParser.h"

class ModManArgs
{
public:
    ModManArgs();
    bool ParseArgs(int argc, const char** argv, bool& shouldContinue);

private:
    /**
     * \brief Prints a command line usage help text for the ModMan tool to stdout.
     */
    void PrintUsage() const;
    static void PrintVersion();

    ArgumentParser m_argument_parser;
};
