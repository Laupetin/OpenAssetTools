#pragma once

#include "RenderingContext.h"

#include <ostream>
#include <string>
#include <vector>

class CodeTemplateFile
{
public:
    CodeTemplateFile(std::string fileName, const int tag)
        : m_file_name(std::move(fileName)),
          m_tag(tag)
    {
    }

    std::string m_file_name;
    int m_tag;
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

    virtual std::vector<CodeTemplateFile> GetFilesToRender(RenderingContext* context) = 0;
    virtual void RenderFile(std::ostream& stream, int fileTag, RenderingContext* context) = 0;
};
