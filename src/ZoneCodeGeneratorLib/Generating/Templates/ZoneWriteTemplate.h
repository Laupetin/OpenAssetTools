#pragma once

#include "Generating/ICodeTemplate.h"

class ZoneWriteTemplate final : public ICodeTemplate
{
public:
    std::vector<CodeTemplateFile> GetFilesToRenderOncePerAsset(const OncePerAssetRenderingContext& context) override;
    void RenderOncePerAssetFile(std::ostream& stream, CodeTemplateFileTag fileTag, const OncePerAssetRenderingContext& context) override;
};
