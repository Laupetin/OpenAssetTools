#pragma once

#include "Utils/Arguments/ArgumentParser.h"

class CompilerArgs
{
    ArgumentParser m_argument_parser;

    /**
     * \brief Prints a command line usage help text for the Compiler tool to stdout.
     */
    static void PrintUsage();

    void SetVerbose(bool isVerbose);

public:
    bool m_verbose;

    CompilerArgs();
    bool ParseArgs(int argc, const char** argv);
};
