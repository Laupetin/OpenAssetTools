#pragma once

#include <vector>

#include "ISearchPath.h"

class SearchPaths final : public ISearchPath
{
    std::vector<ISearchPath*> m_search_paths;
    std::vector<std::unique_ptr<ISearchPath>> m_owned_search_paths;

public:
    using iterator = std::vector<ISearchPath*>::iterator;

    SearchPaths() = default;
    ~SearchPaths() override = default;

    SearchPathOpenFile Open(const std::string& fileName) override;
    std::string GetPath() override;
    void Find(const SearchPathSearchOptions& options, const std::function<void(const std::string&)>& callback) override;

    SearchPaths(const SearchPaths& other) = delete;
    SearchPaths(SearchPaths&& other) noexcept = default;
    SearchPaths& operator=(const SearchPaths& other) = delete;
    SearchPaths& operator=(SearchPaths&& other) noexcept = default;

    /**
     * \brief Adds a search path that gets deleted upon destruction of the \c SearchPaths object.
     * \param searchPath The search path to add.
     */
    void CommitSearchPath(std::unique_ptr<ISearchPath> searchPath);

    /**
     * \brief Adds a search path that does \b NOT get deleted upon destruction of the \c SearchPaths object.
     * \param searchPath The search path to add.
     */
    void IncludeSearchPath(ISearchPath* searchPath);

    /**
     * \brief Removes a search path from the \c SearchPaths object. If the search path was committed then it will \b NOT be deleted when destructing the \c SearchPaths object.
     * \param searchPath  The search path to remove.
     */
    void RemoveSearchPath(ISearchPath* searchPath);

    iterator begin();
    iterator end();
};
