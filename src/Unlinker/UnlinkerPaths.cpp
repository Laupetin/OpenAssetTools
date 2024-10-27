#include "UnlinkerPaths.h"

#include "SearchPath/IWD.h"
#include "SearchPath/SearchPathFilesystem.h"
#include "Utils/StringUtils.h"

#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

bool UnlinkerPaths::LoadUserPaths(const UnlinkerArgs& args)
{
    for (const auto& path : args.m_user_search_paths)
    {
        auto absolutePath = fs::absolute(path);

        if (!fs::is_directory(absolutePath))
        {
            std::cerr << std::format("Could not find directory of search path: \"{}\"\n", path);
            return false;
        }

        auto searchPathName = absolutePath.string();
        m_user_paths.CommitSearchPath(std::make_unique<SearchPathFilesystem>(searchPathName));
        m_specified_user_paths.emplace(std::move(searchPathName));
    }

    std::cout << std::format("{} SearchPaths{}\n", m_specified_user_paths.size(), !m_specified_user_paths.empty() ? ":" : "");
    for (const auto& absoluteSearchPath : m_specified_user_paths)
        std::cout << std::format("  \"{}\"\n", absoluteSearchPath);

    if (!m_specified_user_paths.empty())
        std::cerr << "\n";

    return true;
}

std::unique_ptr<ISearchPath> UnlinkerPaths::GetSearchPathsForZone(const std::string& zonePath)
{
    const auto absoluteZoneDirectory = fs::absolute(std::filesystem::path(zonePath).remove_filename());
    const auto absoluteZoneDirectoryString = absoluteZoneDirectory.string();
    if (m_last_zone_path != absoluteZoneDirectoryString)
    {
        m_last_zone_path = absoluteZoneDirectoryString;
        m_last_zone_search_paths = SearchPaths();
        m_last_zone_search_paths.CommitSearchPath(std::make_unique<SearchPathFilesystem>(absoluteZoneDirectoryString));

        std::filesystem::directory_iterator iterator(absoluteZoneDirectory);
        const auto end = fs::end(iterator);
        for (auto i = fs::begin(iterator); i != end; ++i)
        {
            if (!i->is_regular_file())
                continue;

            auto extension = i->path().extension().string();
            utils::MakeStringLowerCase(extension);
            if (extension == ".iwd")
            {
                auto iwd = iwd::LoadFromFile(i->path().string());
                if (iwd)
                    m_last_zone_search_paths.CommitSearchPath(std::move(iwd));
            }
        }
    }

    auto result = std::make_unique<SearchPaths>();
    result->IncludeSearchPath(&m_last_zone_search_paths);
    result->IncludeSearchPath(&m_user_paths);

    return result;
}
