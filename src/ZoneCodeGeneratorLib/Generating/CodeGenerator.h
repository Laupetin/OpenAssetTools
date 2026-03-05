#pragma once

#include "ICodeTemplate.h"
#include "ZoneCodeGeneratorArguments.h"

#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>

enum class CodeGeneratorOutputResult : std::uint8_t
{
    OUTPUT_WRITTEN,
    OUTPUT_WAS_UP_TO_DATE,
    FAILURE
};

class CodeGenerator
{
public:
    explicit CodeGenerator(const ZoneCodeGeneratorArguments* args);

    bool GenerateCode(const IDataRepository* repository);

private:
    void SetupTemplates();

    CodeGeneratorOutputResult GenerateCodeOncePerTemplate(const OncePerTemplateRenderingContext& context, ICodeTemplate* codeTemplate) const;
    CodeGeneratorOutputResult GenerateCodeOncePerAsset(const OncePerAssetRenderingContext& context, ICodeTemplate* codeTemplate) const;

    static bool GetAssetWithName(const IDataRepository* repository, const std::string& name, StructureInformation*& asset);

    const ZoneCodeGeneratorArguments* m_args;
    std::unordered_map<std::string, std::unique_ptr<ICodeTemplate>> m_template_mapping;
};
