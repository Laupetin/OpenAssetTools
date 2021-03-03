#pragma once

#include <functional>
#include <istream>
#include <memory>

#include "Utils/ObjStream.h"
#include "SearchPathSearchOptions.h"

class ISearchPath
{
public:
    virtual ~ISearchPath() = default;

    /**
     * \brief Opens a file relative to the search path.
     * \param fileName The relative path to the file to open.
     * \return A pointer to an \c IFile object to read the found file or \c nullptr when no file could be found.
     */
    virtual std::unique_ptr<std::istream> Open(const std::string& fileName) = 0;

    /**
     * \brief Returns the path to the search path.
     * \return The path to the search path.
     */
    virtual std::string GetPath() = 0;

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
