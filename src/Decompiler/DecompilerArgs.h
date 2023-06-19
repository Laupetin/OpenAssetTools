#pragma once

#include "Utils/Arguments/ArgumentParser.h"

class DecompilerArgs
{
    ArgumentParser m_argument_parser;

    /**
     * \brief Prints a command line usage help text for the Decompiler tool to stdout.
     */
    static void PrintUsage();

    void SetVerbose(bool isVerbose);

public:
    bool m_verbose;

    DecompilerArgs();
    bool ParseArgs(int argc, const char** argv);
};
