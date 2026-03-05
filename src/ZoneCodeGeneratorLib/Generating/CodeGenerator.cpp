#include "CodeGenerator.h"

#include "Domain/Computations/StructureComputations.h"
#include "Templates/AssetStructTestsTemplate.h"
#include "Templates/ZoneLoadTemplate.h"
#include "Templates/ZoneMarkTemplate.h"
#include "Templates/ZoneWriteTemplate.h"
#include "Utils/Logging/Log.h"
#include "Utils/StringUtils.h"

#include <cstring>
#include <filesystem>
#include <format>
#include <fstream>
#include <sstream>

namespace fs = std::filesystem;

namespace
{
    class CodeGeneratorOutput final
    {
    public:
        explicit CodeGeneratorOutput(fs::path path)
            : m_path(std::move(path)),
              m_open(false),
              m_has_existing_file(false)
        {
        }

        ~CodeGeneratorOutput()
        {
            Close();
        }

        bool Open()
        {
            if (m_open)
                return false;

            auto parentFolder(m_path);
            parentFolder.remove_filename();
            create_directories(parentFolder);

            m_has_existing_file = fs::is_regular_file(m_path);

            if (!m_has_existing_file)
            {
                m_file_stream = std::ofstream(m_path, std::fstream::out | std::fstream::binary);
                if (!m_file_stream.is_open())
                    return false;
            }

            m_open = true;
            return true;
        }

        std::ostream& Stream()
        {
            if (!m_has_existing_file)
                return m_file_stream;

            return m_memory;
        }

        CodeGeneratorOutputResult Close()
        {
            if (!m_open)
                return CodeGeneratorOutputResult::FAILURE;

            m_open = false;

            if (m_has_existing_file)
            {
                const auto renderedContent = std::move(m_memory).str();
                m_memory = std::ostringstream();

                if (!FileIsDirty(renderedContent))
                    return CodeGeneratorOutputResult::OUTPUT_WAS_UP_TO_DATE;

                std::ofstream stream(m_path, std::fstream::out | std::fstream::binary);
                if (!stream.is_open())
                    return CodeGeneratorOutputResult::FAILURE;

                stream.write(renderedContent.data(), renderedContent.size());
                stream.close();
            }

            return CodeGeneratorOutputResult::OUTPUT_WRITTEN;
        }

    private:
        [[nodiscard]] bool FileIsDirty(const std::string& renderedContent) const
        {
            const auto fileSize = static_cast<size_t>(fs::file_size(m_path));
            const size_t contentSize = renderedContent.size();
            if (fileSize != contentSize)
                return true;

            std::ifstream oldFileStream(m_path, std::fstream::in | std::fstream::binary);
            if (!oldFileStream.is_open())
                return true;

            char buffer[4096];
            size_t currentContentOffset = 0;
            while (currentContentOffset < contentSize)
            {
                const auto expectedReadCount = std::min<size_t>(sizeof(buffer), contentSize - currentContentOffset);
                oldFileStream.read(buffer, expectedReadCount);
                if (oldFileStream.gcount() != expectedReadCount)
                {
                    oldFileStream.close();
                    return true;
                }

                if (memcmp(buffer, renderedContent.data() + currentContentOffset, expectedReadCount) != 0)
                {
                    oldFileStream.close();
                    return true;
                }

                currentContentOffset += expectedReadCount;
            }

            oldFileStream.close();
            return false;
        }

        fs::path m_path;

        bool m_open;
        bool m_has_existing_file;
        std::ofstream m_file_stream;
        std::ostringstream m_memory;
    };
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

CodeGeneratorOutputResult CodeGenerator::GenerateCodeOncePerTemplate(const OncePerTemplateRenderingContext& context, ICodeTemplate* codeTemplate) const
{
    bool wroteAtLeastOneFile = false;
    for (const auto& codeFile : codeTemplate->GetFilesToRenderOncePerTemplate(context))
    {
        fs::path outputPath(m_args->m_output_directory);
        outputPath.append(codeFile.m_file_name);

        CodeGeneratorOutput out(outputPath);
        if (!out.Open())
        {
            con::error("Failed to open file '{}'", outputPath.string());
            return CodeGeneratorOutputResult::FAILURE;
        }

        codeTemplate->RenderOncePerTemplateFile(out.Stream(), codeFile.m_tag, context);

        const auto fileResult = out.Close();
        if (fileResult == CodeGeneratorOutputResult::FAILURE)
        {
            con::error("Failed to write file '{}'", outputPath.string());
            return CodeGeneratorOutputResult::FAILURE;
        }

        if (fileResult == CodeGeneratorOutputResult::OUTPUT_WRITTEN)
            wroteAtLeastOneFile = true;
    }

    return wroteAtLeastOneFile ? CodeGeneratorOutputResult::OUTPUT_WRITTEN : CodeGeneratorOutputResult::OUTPUT_WAS_UP_TO_DATE;
}

CodeGeneratorOutputResult CodeGenerator::GenerateCodeOncePerAsset(const OncePerAssetRenderingContext& context, ICodeTemplate* codeTemplate) const
{
    bool wroteAtLeastOneFile = false;
    for (const auto& codeFile : codeTemplate->GetFilesToRenderOncePerAsset(context))
    {
        fs::path outputPath(m_args->m_output_directory);
        outputPath.append(codeFile.m_file_name);

        CodeGeneratorOutput out(outputPath);
        if (!out.Open())
        {
            con::error("Failed to open file '{}'", outputPath.string());
            return CodeGeneratorOutputResult::FAILURE;
        }

        codeTemplate->RenderOncePerAssetFile(out.Stream(), codeFile.m_tag, context);

        const auto fileResult = out.Close();
        if (fileResult == CodeGeneratorOutputResult::FAILURE)
        {
            con::error("Failed to write file '{}'", outputPath.string());
            return CodeGeneratorOutputResult::FAILURE;
        }

        if (fileResult == CodeGeneratorOutputResult::OUTPUT_WRITTEN)
            wroteAtLeastOneFile = true;
    }

    return wroteAtLeastOneFile ? CodeGeneratorOutputResult::OUTPUT_WRITTEN : CodeGeneratorOutputResult::OUTPUT_WAS_UP_TO_DATE;
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
            const auto result = GenerateCodeOncePerAsset(*context, foundTemplate->second.get());
            switch (result)
            {
            case CodeGeneratorOutputResult::OUTPUT_WRITTEN:
                con::info("Successfully generated code for asset '{}' with preset '{}'", asset->m_definition->GetFullName(), foundTemplate->first);
                break;
            case CodeGeneratorOutputResult::OUTPUT_WAS_UP_TO_DATE:
                con::info("Code was up to date for asset '{}' with preset '{}'", asset->m_definition->GetFullName(), foundTemplate->first);
                break;
            case CodeGeneratorOutputResult::FAILURE:
                con::error("Failed to generate code for asset '{}' with preset '{}'", asset->m_definition->GetFullName(), foundTemplate->first);
                return false;
            }
        }

        {
            auto context = OncePerTemplateRenderingContext::BuildContext(repository);
            const auto result = GenerateCodeOncePerTemplate(*context, foundTemplate->second.get());
            switch (result)
            {
            case CodeGeneratorOutputResult::OUTPUT_WRITTEN:
                con::info("Successfully generated code with preset '{}'", foundTemplate->first);
                break;
            case CodeGeneratorOutputResult::OUTPUT_WAS_UP_TO_DATE:
                con::info("Code was up to date for preset '{}'", foundTemplate->first);
                break;
            case CodeGeneratorOutputResult::FAILURE:
                con::error("Failed to generate code with preset '{}'", foundTemplate->first);
                return false;
            }
        }
    }
    const auto end = std::chrono::steady_clock::now();
    con::debug("Generating code took {}ms", std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count());

    return true;
}
