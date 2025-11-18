#pragma once

#include "SearchPathSearchOptions.h"
#include "Utils/ClassUtils.h"

#include <cstdint>
#include <functional>
#include <istream>
#include <memory>
#include <string>

class SearchPathOpenFile
{
public:
    std::unique_ptr<std::istream> m_stream;
    int64_t m_length;

    [[nodiscard]] bool IsOpen() const;

    SearchPathOpenFile();
    SearchPathOpenFile(std::unique_ptr<std::istream> stream, int64_t length);
};

class ISearchPath
{
public:
    ISearchPath() = default;
    virtual ~ISearchPath() = default;

    ISearchPath(const ISearchPath& other) = default;
    ISearchPath(ISearchPath&& other) noexcept = default;
    ISearchPath& operator=(const ISearchPath& other) = default;
    ISearchPath& operator=(ISearchPath&& other) noexcept = default;

    /**
     * \brief Opens a file relative to the search path.
     * \param fileName The relative path to the file to open.
     * \return A pointer to an \c IFile object to read the found file or \c nullptr when no file could be found.
     */
    virtual SearchPathOpenFile Open(const std::string& fileName) = 0;

    /**
     * \brief Returns the path to the search path.
     * \return The path to the search path.
     */
    virtual const std::string& GetPath() = 0;

    /**
     * \brief Iterates through all files of the search path.
     * \param callback The callback to call for each found file with it's path relative to the search path.
     * \param options Options that modify the search.
     */
    virtual void Find(const SearchPathSearchOptions& options, const std::function<void(const std::string&)>& callback) = 0;

    /**
     * \brief Iterates through all files of the search path.
     * \param callback The callback to call for each found file with it's path relative to the search path.
     */
    void Find(const std::function<void(const std::string&)>& callback)
    {
        Find(SearchPathSearchOptions(), callback);
    }
};
