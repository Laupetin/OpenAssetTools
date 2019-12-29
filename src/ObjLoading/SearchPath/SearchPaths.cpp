#include "SearchPaths.h"

SearchPaths::~SearchPaths()
{
    for(auto searchPath : m_search_paths)
    {
        delete searchPath;
    }
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
    for(auto searchPath : m_search_paths)
    {
        auto* file = searchPath->Open(fileName);

        if(file != nullptr)
        {
            return file;
        }
    }

    return nullptr;
}

void SearchPaths::AddSearchPath(ISearchPath* searchPath)
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