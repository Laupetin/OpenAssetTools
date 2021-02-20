#include "ZoneLoadTemplate.h"

#include <sstream>

std::vector<CodeTemplateFile> ZoneLoadTemplate::GetFilesToRender(RenderingContext* context)
{
    std::vector<CodeTemplateFile> files;

    auto assetName = context->m_asset->m_definition->m_name;
    for (auto& c : assetName)
        c = static_cast<char>(tolower(c));

    {
        std::ostringstream str;
        str << assetName << '/' << assetName << "_load_db.h";
        files.emplace_back(str.str(), TAG_HEADER);
    }

    {
        std::ostringstream str;
        str << assetName << '/' << assetName << "_load_db.cpp";
        files.emplace_back(str.str(), TAG_SOURCE);
    }

    return files;
}

void ZoneLoadTemplate::RenderFile(std::ostream& stream, const int fileTag, RenderingContext* context)
{
    if (fileTag == TAG_HEADER)
    {
        stream << "Header";
    }
    else if (fileTag == TAG_SOURCE)
    {
        stream << "Source";
    }
    else
    {
        stream << "Unknown";
    }
}
