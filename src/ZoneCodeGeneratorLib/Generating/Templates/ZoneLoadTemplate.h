#pragma once
#include "Generating/ICodeTemplate.h"

class ZoneLoadTemplate final : public ICodeTemplate
{
public:
    std::vector<CodeTemplateFile> GetFilesToRender(RenderingContext* context) override;
    void RenderFile(std::ostream& stream, int fileTag, RenderingContext* context) override;
};
