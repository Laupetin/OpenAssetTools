#pragma once

#include "ISearchPath.h"
#include <string>

class SearchPathFilesystem final : public ISearchPath
{
    std::string m_path;

public:
    explicit SearchPathFilesystem(std::string path);
    const std::string& GetPath() const;

    FileAPI::IFile* Open(const std::string& fileName) override;
    void FindAll(std::function<void(const std::string&)> callback) override;
    void FindAllOnDisk(std::function<void(const std::string&)> callback) override;
    void FindByExtension(const std::string& extension, std::function<void(const std::string&)> callback) override;
    void FindOnDiskByExtension(const std::string& extension, std::function<void(const std::string&)> callback) override;
};