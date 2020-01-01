#include "SearchPaths.h"

#include <filesystem>

SearchPaths::SearchPaths() = default;

SearchPaths::~SearchPaths()
{
    for(auto searchPathToFree : m_to_free)
    {
        delete searchPathToFree;
    }
    m_to_free.clear();

    m_search_paths.clear();
}

SearchPaths::SearchPaths(const SearchPaths& other)
    : m_search_paths(other.m_search_paths)
{
    
}

SearchPaths::SearchPaths(SearchPaths&& other) noexcept
    : m_search_paths(std::move(other.m_search_paths))
{
    
}

SearchPaths& SearchPaths::operator=(const SearchPaths& other)
{
    m_search_paths = other.m_search_paths;

    return *this;
}

SearchPaths& SearchPaths::operator=(SearchPaths&& other) noexcept
{
    m_search_paths = std::move(other.m_search_paths);

    return *this;
}

FileAPI::IFile* SearchPaths::Open(const std::string& fileName)
{
    for(auto searchPathEntry : m_search_paths)
    {
        auto* file = searchPathEntry->Open(fileName);

        if(file != nullptr)
        {
            return file;
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
    for (auto searchPathEntry : m_search_paths)
    {
        searchPathEntry->Find(options, callback);
    }
}

void SearchPaths::CommitSearchPath(ISearchPath* searchPath)
{
    m_search_paths.push_back(searchPath);
    m_to_free.push_back(searchPath);
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