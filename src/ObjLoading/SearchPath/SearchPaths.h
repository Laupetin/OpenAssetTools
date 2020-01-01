#pragma once

#include "ISearchPath.h"
#include <vector>

class SearchPaths final : public ISearchPath
{
    std::vector<ISearchPath*> m_search_paths;
    std::vector<ISearchPath*> m_to_free;

public:
    using iterator = std::vector<ISearchPath*>::iterator;

    SearchPaths();
    ~SearchPaths() override;

    FileAPI::IFile* Open(const std::string& fileName) override;
    std::string GetPath() override;
    void Find(const SearchPathSearchOptions& options, const std::function<void(const std::string&)>& callback) override;

    SearchPaths(const SearchPaths& other);
    SearchPaths(SearchPaths&& other) noexcept;
    SearchPaths& operator=(const SearchPaths& other);
    SearchPaths& operator=(SearchPaths&& other) noexcept;

    /**
     * \brief Adds a search path that gets deleted upon destruction of the \c SearchPaths object.
     * \param searchPath The search path to add.
     */
    void CommitSearchPath(ISearchPath* searchPath);

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
