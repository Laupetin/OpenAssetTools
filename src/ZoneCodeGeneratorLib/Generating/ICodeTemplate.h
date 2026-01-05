#pragma once

#include "OncePerAssetRenderingContext.h"
#include "OncePerTemplateRenderingContext.h"

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

    virtual std::vector<CodeTemplateFile> GetFilesToRenderOncePerTemplate(const OncePerTemplateRenderingContext& context)
    {
        return {};
    }

    virtual void RenderOncePerTemplateFile(std::ostream& stream, CodeTemplateFileTag fileTag, const OncePerTemplateRenderingContext& context) {}

    virtual std::vector<CodeTemplateFile> GetFilesToRenderOncePerAsset(const OncePerAssetRenderingContext& context)
    {
        return {};
    }

    virtual void RenderOncePerAssetFile(std::ostream& stream, CodeTemplateFileTag fileTag, const OncePerAssetRenderingContext& context) {}
};
