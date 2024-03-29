#pragma once
#include "Generating/ICodeTemplate.h"

class ZoneMarkTemplate final : public ICodeTemplate
{
    static constexpr int TAG_HEADER = 1;
    static constexpr int TAG_SOURCE = 2;

    class Internal;

public:
    std::vector<CodeTemplateFile> GetFilesToRender(RenderingContext* context) override;
    void RenderFile(std::ostream& stream, int fileTag, RenderingContext* context) override;
};
