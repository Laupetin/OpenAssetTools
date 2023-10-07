#include "LinkerSearchPaths.h"

#include <filesystem>
#include <iostream>

#include "ObjLoading.h"
#include "ObjContainer/IWD/IWD.h"
#include "SearchPath/SearchPathFilesystem.h"

namespace fs = std::filesystem;

LinkerSearchPaths::LinkerSearchPaths(const LinkerArgs& args)
    : m_args(args)
{
}

void LinkerSearchPaths::LoadSearchPath(ISearchPath* searchPath) const
{
    if (m_args.m_verbose)
    {
        printf("Loading search path: \"%s\"\n", searchPath->GetPath().c_str());
    }

    ObjLoading::LoadIWDsInSearchPath(searchPath);
}

void LinkerSearchPaths::UnloadSearchPath(ISearchPath* searchPath) const
{
    if (m_args.m_verbose)
    {
        printf("Unloading search path: \"%s\"\n", searchPath->GetPath().c_str());
    }

    ObjLoading::UnloadIWDsInSearchPath(searchPath);
}

SearchPaths LinkerSearchPaths::GetAssetSearchPathsForProject(const std::string& gameName, const std::string& projectName)
{
    SearchPaths searchPathsForProject;

    for (const auto& searchPathStr : m_args.GetAssetSearchPathsForProject(gameName, projectName))
    {
        auto absolutePath = fs::absolute(searchPathStr);

        if (!fs::is_directory(absolutePath))
        {
            if (m_args.m_verbose)
                std::cout << "Adding asset search path (Not found): " << absolutePath.string() << std::endl;
            continue;
        }

        if (m_args.m_verbose)
            std::cout << "Adding asset search path: " << absolutePath.string() << std::endl;

        auto searchPath = std::make_unique<SearchPathFilesystem>(searchPathStr);
        LoadSearchPath(searchPath.get());
        searchPathsForProject.IncludeSearchPath(searchPath.get());
        m_loaded_project_search_paths.emplace_back(std::move(searchPath));
    }

    searchPathsForProject.IncludeSearchPath(&m_asset_search_paths);

    for (auto* iwd : IWD::Repository)
    {
        searchPathsForProject.IncludeSearchPath(iwd);
    }

    return searchPathsForProject;
}

SearchPaths LinkerSearchPaths::GetGdtSearchPathsForProject(const std::string& gameName, const std::string& projectName)
{
    SearchPaths searchPathsForProject;

    for (const auto& searchPathStr : m_args.GetGdtSearchPathsForProject(gameName, projectName))
    {
        auto absolutePath = fs::absolute(searchPathStr);

        if (!fs::is_directory(absolutePath))
        {
            if (m_args.m_verbose)
                std::cout << "Adding gdt search path (Not found): " << absolutePath.string() << std::endl;
            continue;
        }

        if (m_args.m_verbose)
            std::cout << "Adding gdt search path: " << absolutePath.string() << std::endl;

        searchPathsForProject.CommitSearchPath(std::make_unique<SearchPathFilesystem>(searchPathStr));
    }

    searchPathsForProject.IncludeSearchPath(&m_gdt_search_paths);

    return searchPathsForProject;
}

SearchPaths LinkerSearchPaths::GetSourceSearchPathsForProject(const std::string& projectName)
{
    SearchPaths searchPathsForProject;

    for (const auto& searchPathStr : m_args.GetSourceSearchPathsForProject(projectName))
    {
        auto absolutePath = fs::absolute(searchPathStr);

        if (!fs::is_directory(absolutePath))
        {
            if (m_args.m_verbose)
                std::cout << "Adding source search path (Not found): " << absolutePath.string() << std::endl;
            continue;
        }

        if (m_args.m_verbose)
            std::cout << "Adding source search path: " << absolutePath.string() << std::endl;

        searchPathsForProject.CommitSearchPath(std::make_unique<SearchPathFilesystem>(searchPathStr));
    }

    searchPathsForProject.IncludeSearchPath(&m_source_search_paths);

    return searchPathsForProject;
}

bool LinkerSearchPaths::BuildProjectIndependentSearchPaths()
{
    for (const auto& path : m_args.GetProjectIndependentAssetSearchPaths())
    {
        auto absolutePath = fs::absolute(path);

        if (!fs::is_directory(absolutePath))
        {
            if (m_args.m_verbose)
                std::cout << "Adding asset search path (Not found): " << absolutePath.string() << std::endl;
            continue;
        }

        if (m_args.m_verbose)
            std::cout << "Adding asset search path: " << absolutePath.string() << std::endl;

        auto searchPath = std::make_unique<SearchPathFilesystem>(absolutePath.string());
        LoadSearchPath(searchPath.get());
        m_asset_search_paths.CommitSearchPath(std::move(searchPath));
    }

    for (const auto& path : m_args.GetProjectIndependentGdtSearchPaths())
    {
        auto absolutePath = fs::absolute(path);

        if (!fs::is_directory(absolutePath))
        {
            if (m_args.m_verbose)
                std::cout << "Loading gdt search path (Not found): " << absolutePath.string() << std::endl;
            continue;
        }

        if (m_args.m_verbose)
            std::cout << "Adding gdt search path: " << absolutePath.string() << std::endl;

        m_gdt_search_paths.CommitSearchPath(std::make_unique<SearchPathFilesystem>(absolutePath.string()));
    }

    for (const auto& path : m_args.GetProjectIndependentSourceSearchPaths())
    {
        auto absolutePath = fs::absolute(path);

        if (!fs::is_directory(absolutePath))
        {
            if (m_args.m_verbose)
                std::cout << "Loading source search path (Not found): " << absolutePath.string() << std::endl;
            continue;
        }

        if (m_args.m_verbose)
            std::cout << "Adding source search path: " << absolutePath.string() << std::endl;

        m_source_search_paths.CommitSearchPath(std::make_unique<SearchPathFilesystem>(absolutePath.string()));
    }

    return true;
}


void LinkerSearchPaths::UnloadProjectSpecificSearchPaths()
{
    for (const auto& loadedSearchPath : m_loaded_project_search_paths)
    {
        UnloadSearchPath(loadedSearchPath.get());
    }

    m_loaded_project_search_paths.clear();
}
