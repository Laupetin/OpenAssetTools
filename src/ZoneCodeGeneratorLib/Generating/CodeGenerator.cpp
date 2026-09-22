#include "CodeGenerator.h"

#include "Domain/Computations/StructureComputations.h"
#include "Templates/AssetStructTestsTemplate.h"
#include "Templates/ZoneLoadTemplate.h"
#include "Templates/ZoneMarkTemplate.h"
#include "Templates/ZoneWriteTemplate.h"
#include "Utils/FileUtils.h"
#include "Utils/Logging/Log.h"
#include "Utils/StringUtils.h"

#include <filesystem>
#include <format>
#include <fstream>

namespace fs = std::filesystem;

namespace
{
    void LogDirtyCheckResult(const utils::TextFileCheckDirtyResult result, const std::string_view forWhat)
    {
        switch (result)
        {
        case utils::TextFileCheckDirtyResult::OUTPUT_WRITTEN:
            con::info("Successfully generated code for {}", forWhat);
            break;
        case utils::TextFileCheckDirtyResult::OUTPUT_WAS_UP_TO_DATE:
            con::info("Code was up to date for {}", forWhat);
            break;
        case utils::TextFileCheckDirtyResult::FAILURE:
            con::error("Failed to generate code for {}", forWhat);
            break;
        }
    }
} // namespace

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

utils::TextFileCheckDirtyResult CodeGenerator::GenerateCodeOncePerTemplate(const PerTemplateRenderingContext& context, ICodeTemplate* codeTemplate) const
{
    bool wroteAtLeastOneFile = false;
    for (const auto& codeFile : codeTemplate->GetFilesToRenderOncePerTemplate(context))
    {
        fs::path outputPath(m_args->m_output_directory);
        outputPath.append(codeFile.m_file_name);

        utils::TextFileCheckDirtyOutput out(outputPath);
        if (!out.Open())
        {
            con::error("Failed to open file '{}'", outputPath.string());
            return utils::TextFileCheckDirtyResult::FAILURE;
        }

        codeTemplate->RenderOncePerTemplateFile(out.Stream(), codeFile.m_tag, context);

        const auto fileResult = out.Close();
        if (fileResult == utils::TextFileCheckDirtyResult::FAILURE)
        {
            con::error("Failed to write file '{}'", outputPath.string());
            return utils::TextFileCheckDirtyResult::FAILURE;
        }

        if (fileResult == utils::TextFileCheckDirtyResult::OUTPUT_WRITTEN)
            wroteAtLeastOneFile = true;
    }

    return wroteAtLeastOneFile ? utils::TextFileCheckDirtyResult::OUTPUT_WRITTEN : utils::TextFileCheckDirtyResult::OUTPUT_WAS_UP_TO_DATE;
}

utils::TextFileCheckDirtyResult CodeGenerator::GenerateCodeOncePerVariant(const PerVariantRenderingContext& context, ICodeTemplate* codeTemplate) const
{
    bool wroteAtLeastOneFile = false;
    for (const auto& codeFile : codeTemplate->GetFilesToRenderOncePerVariant(context))
    {
        fs::path outputPath(m_args->m_output_directory);
        outputPath.append(codeFile.m_file_name);

        utils::TextFileCheckDirtyOutput out(outputPath);
        if (!out.Open())
        {
            con::error("Failed to open file '{}'", outputPath.string());
            return utils::TextFileCheckDirtyResult::FAILURE;
        }

        codeTemplate->RenderOncePerVariantFile(out.Stream(), codeFile.m_tag, context);

        const auto fileResult = out.Close();
        if (fileResult == utils::TextFileCheckDirtyResult::FAILURE)
        {
            con::error("Failed to write file '{}'", outputPath.string());
            return utils::TextFileCheckDirtyResult::FAILURE;
        }

        if (fileResult == utils::TextFileCheckDirtyResult::OUTPUT_WRITTEN)
            wroteAtLeastOneFile = true;
    }

    return wroteAtLeastOneFile ? utils::TextFileCheckDirtyResult::OUTPUT_WRITTEN : utils::TextFileCheckDirtyResult::OUTPUT_WAS_UP_TO_DATE;
}

utils::TextFileCheckDirtyResult CodeGenerator::GenerateCodeOncePerAsset(const PerAssetRenderingContext& context, ICodeTemplate* codeTemplate) const
{
    bool wroteAtLeastOneFile = false;
    for (const auto& codeFile : codeTemplate->GetFilesToRenderOncePerAsset(context))
    {
        fs::path outputPath(m_args->m_output_directory);
        outputPath.append(codeFile.m_file_name);

        utils::TextFileCheckDirtyOutput out(outputPath);
        if (!out.Open())
        {
            con::error("Failed to open file '{}'", outputPath.string());
            return utils::TextFileCheckDirtyResult::FAILURE;
        }

        codeTemplate->RenderOncePerAssetFile(out.Stream(), codeFile.m_tag, context);

        const auto fileResult = out.Close();
        if (fileResult == utils::TextFileCheckDirtyResult::FAILURE)
        {
            con::error("Failed to write file '{}'", outputPath.string());
            return utils::TextFileCheckDirtyResult::FAILURE;
        }

        if (fileResult == utils::TextFileCheckDirtyResult::OUTPUT_WRITTEN)
            wroteAtLeastOneFile = true;
    }

    return wroteAtLeastOneFile ? utils::TextFileCheckDirtyResult::OUTPUT_WRITTEN : utils::TextFileCheckDirtyResult::OUTPUT_WAS_UP_TO_DATE;
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

        for (const auto* variant : repository->GetGameVariants())
        {
            for (auto* asset : assets)
            {
                auto context = std::make_unique<PerAssetRenderingContext>(repository, asset, variant);
                const auto result = GenerateCodeOncePerAsset(*context, foundTemplate->second.get());
                LogDirtyCheckResult(
                    result,
                    std::format("for asset '{}' with variant '{}' and preset '{}'", asset->m_definition->GetFullName(), variant->m_name, foundTemplate->first));
                if (result == utils::TextFileCheckDirtyResult::FAILURE)
                    return false;
            }

            {
                auto context = std::make_unique<PerVariantRenderingContext>(repository, variant);
                const auto result = GenerateCodeOncePerVariant(*context, foundTemplate->second.get());
                LogDirtyCheckResult(result, std::format("for variant '{}' with preset '{}'", variant->m_name, foundTemplate->first));
                if (result == utils::TextFileCheckDirtyResult::FAILURE)
                    return false;
            }
        }

        {
            auto context = std::make_unique<PerTemplateRenderingContext>(repository);
            const auto result = GenerateCodeOncePerTemplate(*context, foundTemplate->second.get());
            LogDirtyCheckResult(result, std::format("for preset '{}'", foundTemplate->first));
            if (result == utils::TextFileCheckDirtyResult::FAILURE)
                return false;
        }
    }
    const auto end = std::chrono::steady_clock::now();
    const auto timeInMs = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    con::debug("Generating code took {}ms", timeInMs);

    if (!m_args->m_build_log_file.empty())
    {
        std::ofstream buildLogFile(m_args->m_build_log_file);
        if (buildLogFile.is_open())
        {
            buildLogFile << "Generating code took " << timeInMs << "ms\n";
            buildLogFile.close();
        }
        else
        {
            con::error("Failed to open build log file");
        }
    }

    return true;
}
