#pragma once

#include "Generating/ICodeTemplate.h"

class AssetStructTestsTemplate final : public ICodeTemplate
{
public:
    std::vector<CodeTemplateFile> GetFilesToRenderOncePerAsset(const PerAssetRenderingContext& context) override;
    void RenderOncePerAssetFile(std::ostream& stream, CodeTemplateFileTag fileTag, const PerAssetRenderingContext& context) override;
};
