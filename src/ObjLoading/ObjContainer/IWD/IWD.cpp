#include "IWD.h"

#include <unzip.h>
#include <cassert>
#include "Utils/PathUtils.h"
#include "Utils/ZlibFileWrapper.h"
#include <filesystem>
#include "ObjLoading.h"

ObjContainerRepository<IWD, ISearchPath> IWD::Repository;

class IWD::Impl : public ISearchPath, IObjContainer
{
    class IWDEntry
    {
    public:
        std::string m_name;
        unz_file_pos m_file_pos;
    };

    std::string m_path;
    FileAPI::IFile* m_file;
    unzFile m_unz_file;

    std::vector<IWDEntry> m_entries;

public:
    Impl(std::string path, FileAPI::IFile* file)
    {
        m_unz_file = nullptr;
        m_path = std::move(path);
        m_file = file;
    }

    ~Impl()
    {
        if (m_unz_file != nullptr)
        {
            unzClose(m_unz_file);
        }

        if (m_file)
        {
            m_file->Close();
            delete m_file;
            m_file = nullptr;
        }
    }

    bool Initialize()
    {
        auto ioFunctions = ZlibFileWrapper::CreateFunctions32ForFile(m_file);
        m_unz_file = unzOpen2("", &ioFunctions);

        if (m_unz_file == nullptr)
        {
            printf("Could not open IWD \"%s\"\n", m_path.c_str());
            return false;
        }

        auto ret = unzGoToFirstFile(m_unz_file);
        while (ret == Z_OK)
        {
            unz_file_info info;
            char fileNameBuffer[256];
            unzGetCurrentFileInfo(m_unz_file, &info, fileNameBuffer, sizeof fileNameBuffer, nullptr, 0, nullptr, 0);

            std::string fileName(fileNameBuffer);
            std::filesystem::path path(fileName);

            if (path.has_filename())
            {
                IWDEntry entry;
                entry.m_name = std::move(fileName);
                unzGetFilePos(m_unz_file, &entry.m_file_pos);
                m_entries.push_back(entry);
            }

            ret = unzGoToNextFile(m_unz_file);
        }

        if(ObjLoading::Configuration.Verbose)
        {
            printf("Loaded IWD \"%s\" with %u entries\n", m_path.c_str(), m_entries.size());
        }

        return true;
    }

    FileAPI::IFile* Open(const std::string& fileName) override
    {
        if (m_unz_file == nullptr)
        {
            return nullptr;
        }

        // TODO

        return nullptr;
    }

    std::string GetPath() override
    {
        return m_path;
    }

    std::string GetName() override
    {
        return utils::Path::GetFilename(m_path);
    }

    void Find(const SearchPathSearchOptions& options, const std::function<void(const std::string&)>& callback) override
    {
        // TODO
    }
};

IWD::IWD(std::string path, FileAPI::IFile* file)
{
    m_impl = new Impl(std::move(path), file);
}

IWD::~IWD()
{
    delete m_impl;
    m_impl = nullptr;
}

IWD::IWD(IWD&& other) noexcept
{
    m_impl = other.m_impl;
    other.m_impl = nullptr;
}

IWD& IWD::operator=(IWD&& other) noexcept
{
    m_impl = other.m_impl;
    other.m_impl = nullptr;

    return *this;
}

bool IWD::Initialize() const
{
    return m_impl->Initialize();
}

FileAPI::IFile* IWD::Open(const std::string& fileName)
{
    return m_impl->Open(fileName);
}

std::string IWD::GetPath()
{
    return m_impl->GetPath();
}

std::string IWD::GetName()
{
    return m_impl->GetName();
}

void IWD::Find(const SearchPathSearchOptions& options, const std::function<void(const std::string&)>& callback)
{
    return m_impl->Find(options, callback);
}
