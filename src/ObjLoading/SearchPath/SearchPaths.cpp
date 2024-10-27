#include "SearchPaths.h"

#include <cassert>
#include <filesystem>

SearchPathOpenFile SearchPaths::Open(const std::string& fileName)
{
    for (auto* searchPathEntry : m_search_paths)
    {
        auto file = searchPathEntry->Open(fileName);

        if (file.IsOpen())
        {
            return file;
        }
    }

    return SearchPathOpenFile();
}

const std::string& SearchPaths::GetPath()
{
    static const std::string STATIC_NAME = "SearchPaths";
    return STATIC_NAME;
}

void SearchPaths::Find(const SearchPathSearchOptions& options, const std::function<void(const std::string&)>& callback)
{
    for (auto* searchPathEntry : m_search_paths)
    {
        searchPathEntry->Find(options, callback);
    }
}

void SearchPaths::CommitSearchPath(std::unique_ptr<ISearchPath> searchPath)
{
    m_search_paths.push_back(searchPath.get());
    m_owned_search_paths.emplace_back(std::move(searchPath));
}

void SearchPaths::IncludeSearchPath(ISearchPath* searchPath)
{
    assert(searchPath);
    m_search_paths.push_back(searchPath);
}

void SearchPaths::RemoveSearchPath(const ISearchPath* searchPath)
{
    assert(searchPath);
    for (auto i = m_search_paths.begin(); i != m_search_paths.end(); ++i)
    {
        if (*i == searchPath)
        {
            m_search_paths.erase(i);
            return;
        }
    }
}

SearchPaths::iterator SearchPaths::begin()
{
    return m_search_paths.begin();
}

SearchPaths::iterator SearchPaths::end()
{
    return m_search_paths.end();
}
