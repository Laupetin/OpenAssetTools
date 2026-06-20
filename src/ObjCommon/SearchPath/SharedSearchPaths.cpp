#include "SharedSearchPaths.h"

#include "../../ObjLoading/SearchPath/IWD.h"
#include "SearchPathFilesystem.h"
#include "Utils/Logging/Log.h"
#include "Utils/StringUtils.h"

#include <cassert>
#include <filesystem>

namespace fs = std::filesystem;

namespace
{
    std::unique_ptr<ISearchPath> CreateSearchPath(const std::string& searchPathStr)
    {
        auto searchPath = std::make_unique<SearchPathFilesystem>(searchPathStr);
        con::debug("Loaded search path \"{}\"", searchPathStr);

        SearchPaths searchPaths;
        bool hasIwds = false;

        std::filesystem::directory_iterator iterator(searchPathStr);
        const auto end = fs::end(iterator);
        for (auto i = fs::begin(iterator); i != end; ++i)
        {
            if (!i->is_regular_file())
                continue;

            auto extension = i->path().extension().string();
            utils::MakeStringLowerCase(extension);
            if (extension == ".iwd")
            {
                std::string iwdPath = i->path().string();
                auto iwd = iwd::LoadFromFile(iwdPath);
                if (iwd)
                {
                    if (!hasIwds)
                    {
                        searchPaths.CommitSearchPath(std::move(searchPath));
                        hasIwds = true;
                    }

                    searchPaths.CommitSearchPath(std::move(iwd));
                    con::debug("Loaded search path \"{}\"", iwdPath);
                }
            }
        }

        if (hasIwds)
            return std::make_unique<SearchPaths>(std::move(searchPaths));

        return searchPath;
    }
} // namespace

SharedSearchPathEntry::SharedSearchPathEntry(std::unique_ptr<ISearchPath> searchPath)
    : m_search_path(std::move(searchPath)),
      m_ref_count(1)
{
}

SharedSearchPaths::SharedSearchPaths() = default;

ISearchPath& SharedSearchPaths::GetSearchPath()
{
    return m_search_paths;
}

void SharedSearchPaths::RefSearchPath(const std::string& searchPathStr)
{
    const auto existingSearchPath = m_search_path_entries.find(searchPathStr);
    if (existingSearchPath == m_search_path_entries.end())
    {
        auto searchPath = CreateSearchPath(searchPathStr);
        m_search_paths.IncludeSearchPath(searchPath.get());
        m_search_path_entries.emplace(searchPathStr, std::make_unique<SharedSearchPathEntry>(std::move(searchPath)));
    }
    else
    {
        existingSearchPath->second->m_ref_count++;
    }
}

void SharedSearchPaths::UnrefSearchPath(const std::string& searchPathStr)
{
    const auto existingSearchPath = m_search_path_entries.find(searchPathStr);
    if (existingSearchPath != m_search_path_entries.end())
    {
        assert(existingSearchPath->second->m_ref_count > 0);
        const auto newRefCount = --existingSearchPath->second->m_ref_count;

        if (newRefCount == 0)
        {
            m_search_paths.RemoveSearchPath(existingSearchPath->second->m_search_path.get());
            m_search_path_entries.erase(existingSearchPath);
            con::debug("Unloaded search path \"{}\"", searchPathStr);
        }
    }
}
