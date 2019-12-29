#pragma once

#include "ISearchPath.h"
#include <vector>

class SearchPaths final : public ISearchPath
{
    std::vector<ISearchPath*> m_search_paths;

public:
    using iterator = std::vector<ISearchPath*>::iterator;

    ~SearchPaths() override;
    FileAPI::IFile* Open(const std::string& fileName) override;

    SearchPaths(const SearchPaths& other);
    SearchPaths(SearchPaths&& other) noexcept;
    SearchPaths& operator=(const SearchPaths& other);
    SearchPaths& operator=(SearchPaths&& other) noexcept;

    void AddSearchPath(ISearchPath* searchPath);
    void RemoveSearchPath(ISearchPath* searchPath);

    iterator begin();
    iterator end();
};
