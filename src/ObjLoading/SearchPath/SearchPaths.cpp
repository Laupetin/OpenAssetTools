#include "SearchPaths.h"

#include <filesystem>

std::unique_ptr<std::istream> SearchPaths::Open(const std::string& fileName)
{
    for(auto* searchPathEntry : m_search_paths)
    {
        auto file = searchPathEntry->Open(fileName);

        if(file)
        {
            return std::move(file);
        }
    }

    return nullptr;
}

std::string SearchPaths::GetPath()
{
    return "SearchPaths: " + std::to_string(m_search_paths.size()) + " entries";
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
    m_search_paths.push_back(searchPath);
}

void SearchPaths::RemoveSearchPath(ISearchPath* searchPath)
{
    for(auto i = m_search_paths.begin(); i != m_search_paths.end(); ++i)
    {
        if(*i == searchPath)
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