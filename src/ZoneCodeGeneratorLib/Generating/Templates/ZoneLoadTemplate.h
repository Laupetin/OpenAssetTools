#pragma once
#include "Generating/ICodeTemplate.h"

class ZoneLoadTemplate final : public ICodeTemplate
{
public:
    std::vector<CodeTemplateFile> GetFilesToRender(const RenderingContext& context) override;
    void RenderFile(std::ostream& stream, int fileTag, const RenderingContext& context) override;
};
