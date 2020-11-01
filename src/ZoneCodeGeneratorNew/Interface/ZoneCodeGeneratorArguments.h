#pragma once
#include "Utils/Arguments/ArgumentParser.h"

#include <vector>

class ZoneCodeGeneratorArguments
{
    ArgumentParser m_argument_parser;

    /**
     * \brief Prints a command line usage help text for the Unlinker tool to stdout.
     */
    static void PrintUsage();

public:
    enum class ProcessingTask
    {
        GENERATE_CODE,
        PRINT_DATA
    };

    class GenerationTask
    {
    public:
        std::string m_asset_name;
        std::string m_preset_name;

        GenerationTask();
        GenerationTask(std::string assetName, std::string presetName);
    };

    bool m_verbose;

    std::string m_header_path;
    std::string m_commands_path;
    std::string m_output_directory;

    ProcessingTask m_task;
    std::vector<GenerationTask> m_generation_tasks;

    ZoneCodeGeneratorArguments();

    bool Parse(int argc, const char** argv);
};
