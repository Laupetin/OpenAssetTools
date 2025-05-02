#pragma once

#include "ICodeTemplate.h"
#include "ZoneCodeGeneratorArguments.h"

#include <memory>
#include <string>
#include <unordered_map>

class CodeGenerator
{
public:
    explicit CodeGenerator(const ZoneCodeGeneratorArguments* args);

    bool GenerateCode(IDataRepository* repository);

private:
    void SetupTemplates();

    bool GenerateCodeForTemplate(const RenderingContext& context, ICodeTemplate* codeTemplate) const;
    static bool GetAssetWithName(IDataRepository* repository, const std::string& name, StructureInformation*& asset);

    const ZoneCodeGeneratorArguments* m_args;
    std::unordered_map<std::string, std::unique_ptr<ICodeTemplate>> m_template_mapping;
};
