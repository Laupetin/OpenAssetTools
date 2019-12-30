#pragma once

#include "Utils/FileAPI.h"
#include <string>
#include <functional>

class ISearchPath
{
public:
    virtual ~ISearchPath() = default;

    /**
     * \brief Opens a file relative to the search path.
     * \param fileName The relative path to the file to open.
     * \return A pointer to an \c IFile object to read the found file or \c nullptr when no file could be found.
     */
    virtual FileAPI::IFile* Open(const std::string& fileName) = 0;

    /**
     * \brief Iterates through all files of the search path.
     * \param callback The callback to call for each found file with it's path relative to the search path.
     */
    virtual void FindAll(std::function<void(const std::string&)> callback) = 0;

    /**
     * \brief Iterates through all files available through the OS file system.
     * \param callback The callback to call for each found file with it's full path.
     */
    virtual void FindAllOnDisk(std::function<void(const std::string&)> callback) = 0;

    /**
     * \brief Iterates through all files of the search path with the specified extension.
     * \param extension The extension of all files to find.
     * \param callback The callback to call for each found file with it's path relative to the search path.
     */
    virtual void FindByExtension(const std::string& extension, std::function<void(const std::string&)> callback) = 0;

    /**
     * \brief Iterates through all files available through the OS file system with the specified extension.
     * \param extension The extension of all files to find.
     * \param callback The callback to call for each found file with it's full path.
     */
    virtual void FindOnDiskByExtension(const std::string& extension, std::function<void(const std::string&)> callback) = 0;
};
