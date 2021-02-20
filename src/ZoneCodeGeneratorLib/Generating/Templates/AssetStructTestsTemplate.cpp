#include "AssetStructTestsTemplate.h"

#include <sstream>

std::vector<CodeTemplateFile> AssetStructTestsTemplate::GetFilesToRender(RenderingContext* context)
{
    std::vector<CodeTemplateFile> files;

    auto assetName = context->m_asset->m_definition->m_name;
    for (auto& c : assetName)
        c = static_cast<char>(tolower(c));

    {
        std::ostringstream str;
        str << assetName << '/' << assetName << "_struct_test.cpp";
        files.emplace_back(str.str(), TAG_SOURCE);
    }

    return files;
}

void AssetStructTestsTemplate::RenderFile(std::ostream& stream, int fileTag)
{
}
