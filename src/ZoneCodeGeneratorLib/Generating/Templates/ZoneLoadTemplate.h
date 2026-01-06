#pragma once

#include "Generating/ICodeTemplate.h"

class ZoneLoadTemplate final : public ICodeTemplate
{
public:
    std::vector<CodeTemplateFile> GetFilesToRenderOncePerTemplate(const OncePerTemplateRenderingContext& context) override;
    void RenderOncePerTemplateFile(std::ostream& stream, CodeTemplateFileTag fileTag, const OncePerTemplateRenderingContext& context) override;

    std::vector<CodeTemplateFile> GetFilesToRenderOncePerAsset(const OncePerAssetRenderingContext& context) override;
    void RenderOncePerAssetFile(std::ostream& stream, CodeTemplateFileTag fileTag, const OncePerAssetRenderingContext& context) override;
};
