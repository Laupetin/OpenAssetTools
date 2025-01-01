#include "SearchPathMultiInputStream.h"

SearchPathMultiInputStream::SearchPathMultiInputStream(ISearchPath& searchPath)
    : m_search_path(searchPath)
{
}

std::unique_ptr<std::istream> SearchPathMultiInputStream::OpenIncludedFile(const std::string& filename, const std::string& sourceFile)
{
    auto foundFileToInclude = m_search_path.Open(filename);
    if (!foundFileToInclude.IsOpen() || !foundFileToInclude.m_stream)
        return nullptr;

    return std::move(foundFileToInclude.m_stream);
}
