#pragma once

#include "Generating/ICodeTemplate.h"

class ZoneLoadTemplate final : public ICodeTemplate
{
public:
    std::vector<CodeTemplateFile> GetFilesToRenderOncePerVariant(const PerVariantRenderingContext& context) override;
    void RenderOncePerVariantFile(std::ostream& stream, CodeTemplateFileTag fileTag, const PerVariantRenderingContext& context) override;

    std::vector<CodeTemplateFile> GetFilesToRenderOncePerAsset(const PerAssetRenderingContext& context) override;
    void RenderOncePerAssetFile(std::ostream& stream, CodeTemplateFileTag fileTag, const PerAssetRenderingContext& context) override;
};
