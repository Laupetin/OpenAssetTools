#include "ZoneCodeGenerator.h"

#include <cstdio>
#include <memory>
#include <string>
#include <iostream>

#include "ZoneCodeGeneratorArguments.h"
#include "Parsing/Commands/CommandsFileReader.h"
#include "Parsing/Header/HeaderFileReader.h"
#include "Persistence/IDataRepository.h"
#include "Persistence/InMemory/InMemoryRepository.h"
#include "Printing/PrettyPrinter.h"

class ZoneCodeGenerator::Impl
{
    ZoneCodeGeneratorArguments m_args;
    std::unique_ptr<IDataRepository> m_repository;

    bool ReadHeaderData()
    {
        for (const auto& headerFile : m_args.m_header_paths)
        {
            HeaderFileReader headerFileReader(&m_args, headerFile);

            if (!headerFileReader.ReadHeaderFile(m_repository.get()))
                return false;
        }

        return true;
    }

    bool ReadCommandsData()
    {
        for (const auto& commandsFile : m_args.m_command_paths)
        {
            CommandsFileReader commandsFileReader(&m_args, commandsFile);

            if (!commandsFileReader.ReadCommandsFile(m_repository.get()))
                return false;
        }

        return true;
    }

    void PrintData() const
    {
        PrettyPrinter prettyPrinter;
        prettyPrinter.Print(m_repository.get());
    }

    bool GenerateCode()
    {
        for(const auto& generationTask : m_args.m_generation_tasks)
        {
            // TODO: Implement
            std::cout << "Generating code for asset \"" << generationTask.m_asset_name << "\" and preset \"" << generationTask.m_preset_name << "\" ..." << std::endl;
        }

        return true;
    }

public:
    Impl()
    {
        m_repository = std::make_unique<InMemoryRepository>();
    }

    int Run(const int argc, const char** argv)
    {
        if (!m_args.Parse(argc, argv))
            return 1;

        if (!ReadHeaderData() || !ReadCommandsData())
            return 1;
        
        switch(m_args.m_task)
        {
        case ZoneCodeGeneratorArguments::ProcessingTask::PRINT_DATA:
            PrintData();
            return 0;

        case ZoneCodeGeneratorArguments::ProcessingTask::GENERATE_CODE:
            return GenerateCode() ? 0 : 1;

        default:
            std::cout << "Unknown task: " << static_cast<int>(m_args.m_task) << std::endl; 
            return 2;
        }
    }
};

ZoneCodeGenerator::ZoneCodeGenerator()
{
    m_impl = new Impl();
}

ZoneCodeGenerator::~ZoneCodeGenerator()
{
    delete m_impl;
    m_impl = nullptr;
}

int ZoneCodeGenerator::Run(const int argc, const char** argv) const
{
    return m_impl->Run(argc, argv);
}
