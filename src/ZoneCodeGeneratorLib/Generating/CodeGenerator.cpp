#include "CodeGenerator.h"

#include "Domain/Computations/StructureComputations.h"
#include "Templates/AssetStructTestsTemplate.h"
#include "Templates/ZoneLoadTemplate.h"
#include "Templates/ZoneMarkTemplate.h"
#include "Templates/ZoneWriteTemplate.h"

#include <filesystem>
#include <fstream>
#include <iostream>

namespace fs = std::filesystem;

CodeGenerator::CodeGenerator(const ZoneCodeGeneratorArguments* args)
    : m_args(args)
{
    SetupTemplates();
}

void CodeGenerator::SetupTemplates()
{
    m_template_mapping["zoneload"] = std::make_unique<ZoneLoadTemplate>();
    m_template_mapping["zonemark"] = std::make_unique<ZoneMarkTemplate>();
    m_template_mapping["zonewrite"] = std::make_unique<ZoneWriteTemplate>();
    m_template_mapping["assetstructtests"] = std::make_unique<AssetStructTestsTemplate>();
}

bool CodeGenerator::GenerateCodeForTemplate(RenderingContext* context, ICodeTemplate* codeTemplate) const
{
    for (const auto& codeFile : codeTemplate->GetFilesToRender(context))
    {
        fs::path p(m_args->m_output_directory);
        p.append(codeFile.m_file_name);

        auto parentFolder(p);
        parentFolder.remove_filename();
        create_directories(parentFolder);

        std::ofstream stream(p, std::fstream::out | std::fstream::binary);

        if (!stream.is_open())
        {
            std::cout << "Failed to open file '" << p.string() << "'\n";
            return false;
        }

        codeTemplate->RenderFile(stream, codeFile.m_tag, context);

        stream.close();
    }

    return true;
}

bool CodeGenerator::GetAssetWithName(IDataRepository* repository, const std::string& name, StructureInformation*& asset)
{
    auto* def = repository->GetDataDefinitionByName(name);
    if (def == nullptr)
    {
        std::cout << "Could not find type with name '" << name << "'\n";
        return false;
    }

    auto* defWithMembers = dynamic_cast<DefinitionWithMembers*>(def);
    auto* info = defWithMembers != nullptr ? repository->GetInformationFor(defWithMembers) : nullptr;
    if (info == nullptr)
    {
        std::cout << "Could not find type with name '" << name << "'\n";
        return false;
    }

    if (!StructureComputations(info).IsAsset())
    {
        std::cout << "Type is not an asset '" << name << "'\n";
        return false;
    }

    return true;
}

bool CodeGenerator::GenerateCode(IDataRepository* repository)
{
    std::vector<StructureInformation*> assets;

    for (auto* info : repository->GetAllStructureInformation())
    {
        StructureComputations computations(info);
        if (computations.IsAsset())
            assets.push_back(info);
    }

    const auto start = std::chrono::steady_clock::now();
    for (const auto& generationTask : m_args->m_generation_tasks)
    {
        auto templateName = generationTask.m_template_name;
        for (auto& c : templateName)
            c = static_cast<char>(tolower(c));

        const auto foundTemplate = m_template_mapping.find(templateName);
        if (foundTemplate == m_template_mapping.end())
        {
            std::cout << "Unknown template '" << generationTask.m_template_name << "'.\n";
            return false;
        }

        if (generationTask.m_all_assets)
        {
            for (auto* asset : assets)
            {
                auto context = RenderingContext::BuildContext(repository, asset);
                if (!GenerateCodeForTemplate(context.get(), foundTemplate->second.get()))
                {
                    std::cout << "Failed to generate code for asset '" << asset->m_definition->GetFullName() << "' with preset '" << foundTemplate->first
                              << "'\n";
                    return false;
                }

                std::cout << "Successfully generated code for asset '" << asset->m_definition->GetFullName() << "' with preset '" << foundTemplate->first
                          << "'\n";
            }
        }
        else
        {
            StructureInformation* asset;
            if (!GetAssetWithName(repository, generationTask.m_asset_name, asset))
                return false;

            auto context = RenderingContext::BuildContext(repository, asset);
            if (!GenerateCodeForTemplate(context.get(), foundTemplate->second.get()))
                return false;
        }
    }
    const auto end = std::chrono::steady_clock::now();
    if (m_args->m_verbose)
    {
        std::cout << "Generating code took " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms\n";
    }

    return true;
}
