#include "RawTemplater.h"

#include "RawTemplaterArguments.h"
#include "Templating/Templater.h"

#include <filesystem>
#include <fstream>
#include <iostream>

namespace fs = std::filesystem;

class RawTemplater::Impl
{
    RawTemplaterArguments m_args;

    bool m_write_build_log;
    std::ofstream m_build_log_file;

    _NODISCARD bool GenerateCode(const std::string& filename)
    {
        std::ifstream file(filename, std::ios::in | std::ios::binary);
        if (!file.is_open())
        {
            std::cerr << "Failed to open file \"" << filename << "\"\n";
            return false;
        }

        templating::Templater templater(file, filename);
        if (m_write_build_log)
            templater.SetBuildLogFile(&m_build_log_file);
        if (!m_args.m_output_directory.empty())
            return templater.TemplateToDirectory(m_args.m_output_directory);

        const fs::path filePath(filename);
        const auto parentPath = filePath.parent_path();

        return templater.TemplateToDirectory(parentPath.string());
    }

public:
    Impl()
        : m_write_build_log(false)
    {
    }

    int Run(const int argc, const char** argv)
    {
        auto shouldContinue = true;
        if (!m_args.ParseArgs(argc, argv, shouldContinue))
            return 1;

        if (!shouldContinue)
            return 0;

        if (!m_args.m_build_log_file.empty())
        {
            fs::path p = fs::path(m_args.m_build_log_file).parent_path();
            if (!p.empty())
                fs::create_directories(p);

            m_build_log_file = std::ofstream(m_args.m_build_log_file, std::ios::out | std::ios::binary);
            if (!m_build_log_file.is_open())
            {
                std::cerr << "Failed to open build log file \"" << m_args.m_build_log_file << "\"\n";
                return false;
            }
            m_write_build_log = true;
        }

        for (const auto& inputFile : m_args.m_input_files)
        {
            if (!GenerateCode(inputFile))
                return 1;
        }

        return 0;
    }
};

RawTemplater::RawTemplater()
{
    m_impl = new Impl();
}

RawTemplater::~RawTemplater()
{
    delete m_impl;
    m_impl = nullptr;
}

int RawTemplater::Run(const int argc, const char** argv) const
{
    return m_impl->Run(argc, argv);
}
