#include "LinkerSearchPaths.h"

#include "ObjContainer/IWD/IWD.h"
#include "ObjLoading.h"
#include "SearchPath/SearchPathFilesystem.h"

#include <filesystem>
#include <format>
#include <iostream>

namespace fs = std::filesystem;

LinkerSearchPaths::LinkerSearchPaths(const LinkerArgs& args)
    : m_args(args)
{
}

void LinkerSearchPaths::LoadSearchPath(ISearchPath& searchPath) const
{
    if (m_args.m_verbose)
    {
        std::cout << std::format("Loading search path: \"{}\"\n", searchPath.GetPath());
    }

    ObjLoading::LoadIWDsInSearchPath(searchPath);
}

void LinkerSearchPaths::UnloadSearchPath(ISearchPath& searchPath) const
{
    if (m_args.m_verbose)
    {
        std::cout << std::format("Unloading search path: \"{}\"\n", searchPath.GetPath());
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
                std::cout << std::format("Adding asset search path (Not found): {}\n", absolutePath.string());
            continue;
        }

        if (m_args.m_verbose)
            std::cout << std::format("Adding asset search path: {}\n", absolutePath.string());

        auto searchPath = std::make_unique<SearchPathFilesystem>(searchPathStr);
        LoadSearchPath(*searchPath);
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
                std::cout << std::format("Adding gdt search path (Not found): {}\n", absolutePath.string());
            continue;
        }

        if (m_args.m_verbose)
            std::cout << std::format("Adding gdt search path: {}\n", absolutePath.string());

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
                std::cout << std::format("Adding source search path (Not found): {}\n", absolutePath.string());
            continue;
        }

        if (m_args.m_verbose)
            std::cout << std::format("Adding source search path: {}\n", absolutePath.string());

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
                std::cout << std::format("Adding asset search path (Not found): {}\n", absolutePath.string());
            continue;
        }

        if (m_args.m_verbose)
            std::cout << std::format("Adding asset search path: {}\n", absolutePath.string());

        auto searchPath = std::make_unique<SearchPathFilesystem>(absolutePath.string());
        LoadSearchPath(*searchPath);
        m_asset_search_paths.CommitSearchPath(std::move(searchPath));
    }

    for (const auto& path : m_args.GetProjectIndependentGdtSearchPaths())
    {
        auto absolutePath = fs::absolute(path);

        if (!fs::is_directory(absolutePath))
        {
            if (m_args.m_verbose)
                std::cout << std::format("Loading gdt search path (Not found): {}\n", absolutePath.string());
            continue;
        }

        if (m_args.m_verbose)
            std::cout << std::format("Adding gdt search path: {}\n", absolutePath.string());

        m_gdt_search_paths.CommitSearchPath(std::make_unique<SearchPathFilesystem>(absolutePath.string()));
    }

    for (const auto& path : m_args.GetProjectIndependentSourceSearchPaths())
    {
        auto absolutePath = fs::absolute(path);

        if (!fs::is_directory(absolutePath))
        {
            if (m_args.m_verbose)
                std::cout << std::format("Loading source search path (Not found): {}\n", absolutePath.string());
            continue;
        }

        if (m_args.m_verbose)
            std::cout << std::format("Adding source search path: {}\n", absolutePath.string());

        m_source_search_paths.CommitSearchPath(std::make_unique<SearchPathFilesystem>(absolutePath.string()));
    }

    return true;
}

void LinkerSearchPaths::UnloadProjectSpecificSearchPaths()
{
    for (const auto& loadedSearchPath : m_loaded_project_search_paths)
    {
        UnloadSearchPath(*loadedSearchPath);
    }

    m_loaded_project_search_paths.clear();
}
