#pragma once

#include "SearchPath/SearchPaths.h"
#include "UnlinkerArgs.h"

#include <string>
#include <unordered_set>

class UnlinkerPaths
{
public:
    bool LoadUserPaths(const UnlinkerArgs& args);
    std::unique_ptr<ISearchPath> GetSearchPathsForZone(const std::string& zonePath);

private:
    std::string m_last_zone_path;
    SearchPaths m_last_zone_search_paths;

    std::unordered_set<std::string> m_specified_user_paths;
    SearchPaths m_user_paths;
};
