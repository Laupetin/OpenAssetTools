#include "RawTemplater.h"

#include <fstream>
#include <iostream>
#include <filesystem>

#include "RawTemplaterArguments.h"
#include "Templating/Templater.h"

namespace fs = std::filesystem;

class RawTemplater::Impl
{
    RawTemplaterArguments m_args;

    _NODISCARD bool GenerateCode(const std::string& filename) const
    {
        std::ifstream file(filename);
        if (!file.is_open())
        {
            std::cerr << "Failed to open file \"" << filename << "\"\n";
            return false;
        }

        templating::Templater templater(file, filename);
        if (!m_args.m_output_directory.empty())
            return templater.TemplateToDirectory(m_args.m_output_directory);

        const fs::path filePath(filename);
        const auto parentPath = filePath.parent_path();

        return templater.TemplateToDirectory(parentPath.string());
    }

public:
    Impl()
    = default;

    int Run(const int argc, const char** argv)
    {
        if (!m_args.Parse(argc, argv))
            return 1;

        for(const auto& inputFile : m_args.m_input_files)
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
