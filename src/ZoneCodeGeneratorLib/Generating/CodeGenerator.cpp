#include "CodeGenerator.h"

#include "Domain/Computations/StructureComputations.h"
#include "Templates/AssetStructTestsTemplate.h"
#include "Templates/ZoneLoadTemplate.h"
#include "Templates/ZoneMarkTemplate.h"
#include "Templates/ZoneWriteTemplate.h"
#include "Utils/Logging/Log.h"
#include "Utils/StringUtils.h"

#include <filesystem>
#include <format>
#include <fstream>

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

bool CodeGenerator::GenerateCodeOncePerTemplate(const OncePerTemplateRenderingContext& context, ICodeTemplate* codeTemplate) const
{
    for (const auto& codeFile : codeTemplate->GetFilesToRenderOncePerTemplate(context))
    {
        fs::path p(m_args->m_output_directory);
        p.append(codeFile.m_file_name);

        auto parentFolder(p);
        parentFolder.remove_filename();
        create_directories(parentFolder);

        std::ofstream stream(p, std::fstream::out | std::fstream::binary);

        if (!stream.is_open())
        {
            con::error("Failed to open file '{}'", p.string());
            return false;
        }

        codeTemplate->RenderOncePerTemplateFile(stream, codeFile.m_tag, context);

        stream.close();
    }

    return true;
}

bool CodeGenerator::GenerateCodeOncePerAsset(const OncePerAssetRenderingContext& context, ICodeTemplate* codeTemplate) const
{
    for (const auto& codeFile : codeTemplate->GetFilesToRenderOncePerAsset(context))
    {
        fs::path p(m_args->m_output_directory);
        p.append(codeFile.m_file_name);

        auto parentFolder(p);
        parentFolder.remove_filename();
        create_directories(parentFolder);

        std::ofstream stream(p, std::fstream::out | std::fstream::binary);

        if (!stream.is_open())
        {
            con::error("Failed to open file '{}'", p.string());
            return false;
        }

        codeTemplate->RenderOncePerAssetFile(stream, codeFile.m_tag, context);

        stream.close();
    }

    return true;
}

bool CodeGenerator::GetAssetWithName(const IDataRepository* repository, const std::string& name, StructureInformation*& asset)
{
    auto* def = repository->GetDataDefinitionByName(name);
    if (def == nullptr)
    {
        con::error("Could not find type with name '{}'", name);
        return false;
    }

    const auto* defWithMembers = dynamic_cast<DefinitionWithMembers*>(def);
    asset = defWithMembers != nullptr ? repository->GetInformationFor(defWithMembers) : nullptr;
    if (asset == nullptr)
    {
        con::error("Could not find type with name '{}'", name);
        return false;
    }

    if (!StructureComputations(asset).IsAsset())
    {
        con::error("Type is not an asset '{}'", name);
        return false;
    }

    return true;
}

bool CodeGenerator::GenerateCode(const IDataRepository* repository)
{
    std::vector<StructureInformation*> assets;

    for (auto* info : repository->GetAllStructureInformation())
    {
        StructureComputations computations(info);
        if (computations.IsAsset())
            assets.push_back(info);
    }

    const auto start = std::chrono::steady_clock::now();
    for (const auto& templateName : m_args->m_template_names)
    {
        std::string lowerTemplateName(templateName);
        utils::MakeStringLowerCase(lowerTemplateName);

        const auto foundTemplate = m_template_mapping.find(lowerTemplateName);
        if (foundTemplate == m_template_mapping.end())
        {
            con::error("Unknown template '{}'", templateName);
            return false;
        }

        for (auto* asset : assets)
        {
            auto context = OncePerAssetRenderingContext::BuildContext(repository, asset);
            if (!GenerateCodeOncePerAsset(*context, foundTemplate->second.get()))
            {
                con::error("Failed to generate code for asset '{}' with preset '{}'", asset->m_definition->GetFullName(), foundTemplate->first);
                return false;
            }

            con::info("Successfully generated code for asset '{}' with preset '{}'", asset->m_definition->GetFullName(), foundTemplate->first);
        }

        {
            auto context = OncePerTemplateRenderingContext::BuildContext(repository);
            if (!GenerateCodeOncePerTemplate(*context, foundTemplate->second.get()))
            {
                con::error("Failed to generate code with preset '{}'", foundTemplate->first);
                return false;
            }

            con::info("Successfully generated code with preset '{}'", foundTemplate->first);
        }
    }
    const auto end = std::chrono::steady_clock::now();
    con::debug("Generating code took {}ms", std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count());

    return true;
}
