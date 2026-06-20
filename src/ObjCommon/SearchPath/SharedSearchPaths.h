#pragma once

#include "SearchPaths.h"

#include <memory>
#include <unordered_map>

class SharedSearchPathEntry
{
public:
    explicit SharedSearchPathEntry(std::unique_ptr<ISearchPath> searchPath);

    std::unique_ptr<ISearchPath> m_search_path;
    unsigned m_ref_count;
};

class SharedSearchPaths
{
public:
    SharedSearchPaths();

    [[nodiscard]] ISearchPath& GetSearchPath();

    void RefSearchPath(const std::string& searchPathStr);
    void UnrefSearchPath(const std::string& searchPathStr);

private:
    SearchPaths m_search_paths;
    std::unordered_map<std::string, std::unique_ptr<SharedSearchPathEntry>> m_search_path_entries;
};
