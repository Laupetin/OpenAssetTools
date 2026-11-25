#pragma once

#include "PerAssetRenderingContext.h"
#include "PerTemplateRenderingContext.h"

#include <ostream>
#include <string>
#include <vector>

typedef unsigned CodeTemplateFileTag;

class CodeTemplateFile
{
public:
    CodeTemplateFile(std::string fileName, const CodeTemplateFileTag tag)
        : m_file_name(std::move(fileName)),
          m_tag(tag)
    {
    }

    std::string m_file_name;
    CodeTemplateFileTag m_tag;
};

class ICodeTemplate
{
public:
    ICodeTemplate() = default;
    virtual ~ICodeTemplate() = default;
    ICodeTemplate(const ICodeTemplate& other) = default;
    ICodeTemplate(ICodeTemplate&& other) noexcept = default;
    ICodeTemplate& operator=(const ICodeTemplate& other) = default;
    ICodeTemplate& operator=(ICodeTemplate&& other) noexcept = default;

    virtual std::vector<CodeTemplateFile> GetFilesToRenderOncePerTemplate(const PerTemplateRenderingContext& context)
    {
        return {};
    }

    virtual void RenderOncePerTemplateFile(std::ostream& stream, CodeTemplateFileTag fileTag, const PerTemplateRenderingContext& context) {}

    virtual std::vector<CodeTemplateFile> GetFilesToRenderOncePerVariant(const PerVariantRenderingContext& context)
    {
        return {};
    }

    virtual void RenderOncePerVariantFile(std::ostream& stream, CodeTemplateFileTag fileTag, const PerVariantRenderingContext& context) {}

    virtual std::vector<CodeTemplateFile> GetFilesToRenderOncePerAsset(const PerAssetRenderingContext& context)
    {
        return {};
    }

    virtual void RenderOncePerAssetFile(std::ostream& stream, CodeTemplateFileTag fileTag, const PerAssetRenderingContext& context) {}
};
