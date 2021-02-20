#include "ZoneWriteTemplate.h"

#include <sstream>

std::vector<CodeTemplateFile> ZoneWriteTemplate::GetFilesToRender(RenderingContext* context)
{
    std::vector<CodeTemplateFile> files;

    auto assetName = context->m_asset->m_definition->m_name;
    for (auto& c : assetName)
        c = static_cast<char>(tolower(c));

    {
        std::ostringstream str;
        str << assetName << '/' << assetName << "_write_db.h";
        files.emplace_back(str.str(), TAG_HEADER);
    }

    {
        std::ostringstream str;
        str << assetName << '/' << assetName << "_write_db.cpp";
        files.emplace_back(str.str(), TAG_SOURCE);
    }

    return files;
}

void ZoneWriteTemplate::RenderFile(std::ostream& stream, const int fileTag, RenderingContext* context)
{
}
