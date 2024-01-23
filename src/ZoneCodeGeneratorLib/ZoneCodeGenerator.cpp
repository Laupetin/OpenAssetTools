#include "ZoneCodeGenerator.h"

#include "Generating/CodeGenerator.h"
#include "Parsing/Commands/CommandsFileReader.h"
#include "Parsing/Header/HeaderFileReader.h"
#include "Persistence/IDataRepository.h"
#include "Persistence/InMemory/InMemoryRepository.h"
#include "Printing/PrettyPrinter.h"
#include "ZoneCodeGeneratorArguments.h"

#include <cstdio>
#include <iostream>
#include <memory>
#include <string>

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
        const PrettyPrinter prettyPrinter(std::cout, m_repository.get());
        prettyPrinter.PrintAll();
    }

    _NODISCARD bool GenerateCode() const
    {
        CodeGenerator codeGenerator(&m_args);
        return codeGenerator.GenerateCode(m_repository.get());
    }

public:
    Impl()
    {
        m_repository = std::make_unique<InMemoryRepository>();
    }

    int Run(const int argc, const char** argv)
    {
        auto shouldContinue = true;
        if (!m_args.ParseArgs(argc, argv, shouldContinue))
            return 1;

        if (!shouldContinue)
            return 0;

        if (!ReadHeaderData() || !ReadCommandsData())
            return 1;

        if (m_args.ShouldPrint())
        {
            PrintData();
        }

        if (m_args.ShouldGenerate())
        {
            if (!GenerateCode())
                return 1;
        }

        return 0;
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
