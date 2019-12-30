#include "IWD.h"

IWD::IWD(std::string path)
{
    m_path = std::move(path);
}

FileAPI::IFile* IWD::Open(const std::string& fileName)
{
    // TODO
    return nullptr;
}

void IWD::FindAll(std::function<void(const std::string&)> callback)
{
    // TODO
}

void IWD::FindAllOnDisk(std::function<void(const std::string&)> callback)
{
    // Files inside an IWD are not on the disk's file system directly. Therefore do nothing here.
}

void IWD::FindByExtension(const std::string& extension, std::function<void(const std::string&)> callback)
{
    // TODO
}

void IWD::FindOnDiskByExtension(const std::string& extension, std::function<void(const std::string&)> callback)
{
    // Files inside an IWD are not on the disk's file system directly. Therefore do nothing here.
}
