#pragma once

#include "Utils/Arguments/ArgumentParser.h"

#include <string>
#include <vector>

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

class ZoneCodeGeneratorArguments
{
public:
    ZoneCodeGeneratorArguments();
    bool ParseArgs(int argc, const char** argv, bool& shouldContinue);

    [[nodiscard]] bool ShouldGenerate() const;
    [[nodiscard]] bool ShouldPrint() const;

    std::vector<std::string> m_header_paths;
    std::vector<std::string> m_command_paths;
    std::string m_output_directory;

    std::vector<GenerationTask> m_generation_tasks;

private:
    void PrintUsage() const;
    static void PrintVersion();

    ArgumentParser m_argument_parser;
    unsigned m_task_flags;
};
