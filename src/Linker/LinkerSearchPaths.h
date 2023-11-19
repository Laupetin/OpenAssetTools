#pragma once
#include "LinkerArgs.h"
#include "SearchPath/SearchPaths.h"

#include <memory>
#include <vector>

class LinkerSearchPaths
{
public:
    explicit LinkerSearchPaths(const LinkerArgs& args);

    /**
     * \brief Loads a search path.
     * \param searchPath The search path to load.
     */
    void LoadSearchPath(ISearchPath* searchPath) const;

    /**
     * \brief Unloads a search path.
     * \param searchPath The search path to unload.
     */
    void UnloadSearchPath(ISearchPath* searchPath) const;

    SearchPaths GetAssetSearchPathsForProject(const std::string& gameName, const std::string& projectName);

    SearchPaths GetGdtSearchPathsForProject(const std::string& gameName, const std::string& projectName);

    SearchPaths GetSourceSearchPathsForProject(const std::string& projectName);

    /**
     * \brief Initializes the Linker object's search paths based on the user's input.
     * \return \c true if building the search paths was successful, otherwise \c false.
     */
    bool BuildProjectIndependentSearchPaths();

    void UnloadProjectSpecificSearchPaths();

private:
    const LinkerArgs& m_args;
    std::vector<std::unique_ptr<ISearchPath>> m_loaded_project_search_paths;
    SearchPaths m_asset_search_paths;
    SearchPaths m_gdt_search_paths;
    SearchPaths m_source_search_paths;
};
