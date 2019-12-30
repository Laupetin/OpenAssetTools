#pragma once

#include "SearchPath/ISearchPath.h"

class IWD final : public ISearchPath
{
    std::string m_path;

public:
    explicit IWD(std::string path);

    FileAPI::IFile* Open(const std::string& fileName) override;
    void FindAll(std::function<void(const std::string&)> callback) override;
    void FindAllOnDisk(std::function<void(const std::string&)> callback) override;
    void FindByExtension(const std::string& extension, std::function<void(const std::string&)> callback) override;
    void FindOnDiskByExtension(const std::string& extension, std::function<void(const std::string&)> callback) override;
};