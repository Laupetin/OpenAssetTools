#pragma once

#include "Utils/Arguments/ArgumentParser.h"
#include "Utils/ClassUtils.h"

#include <vector>

class ZoneCodeGeneratorArguments
{
    ArgumentParser m_argument_parser;

    /**
     * \brief Prints a command line usage help text for the Unlinker tool to stdout.
     */
    void PrintUsage() const;
    static void PrintVersion();

public:
    static constexpr unsigned FLAG_TASK_GENERATE = 1 << 0;
    static constexpr unsigned FLAG_TASK_PRINT = 1 << 1;

    class GenerationTask
    {
    public:
        bool m_all_assets;
        std::string m_asset_name;
        std::string m_template_name;

        GenerationTask();
        explicit GenerationTask(std::string templateName);
        GenerationTask(std::string assetName, std::string templateName);
    };

    bool m_verbose;

    std::vector<std::string> m_header_paths;
    std::vector<std::string> m_command_paths;
    std::string m_output_directory;

    unsigned m_task_flags;
    std::vector<GenerationTask> m_generation_tasks;

    ZoneCodeGeneratorArguments();
    bool ParseArgs(int argc, const char** argv, bool& shouldContinue);

    _NODISCARD bool ShouldGenerate() const;
    _NODISCARD bool ShouldPrint() const;
};
