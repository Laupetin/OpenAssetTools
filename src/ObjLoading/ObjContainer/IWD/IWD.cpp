#include "IWD.h"

#include "Utils/PathUtils.h"
#include "ObjLoading.h"
#include "Utils/FileToZlibWrapper.h"

#include <unzip.h>
#include <filesystem>
#include <cassert>
#include <map>

ObjContainerRepository<IWD, ISearchPath> IWD::Repository;

class IWDFile final : public FileAPI::IFile
{
public:
    class IParent
    {
    public:
        virtual ~IParent() = default;

        virtual void OnIWDFileClose() = 0;
    };

private:
    IParent* m_parent;
    bool m_open;
    size_t m_size;
    unzFile m_container;

public:
    IWDFile(IParent* parent, const unzFile container, const size_t size)
    {
        m_parent = parent;
        m_container = container;
        m_size = size;
        m_open = true;
    }

    ~IWDFile() override
    {
        if(m_open)
        {
            Close();
        }
    }

    bool IsOpen() override
    {
        return m_open;
    }

    size_t Read(void* buffer, const size_t elementSize, const size_t elementCount) override
    {
        const auto result = unzReadCurrentFile(m_container, buffer, elementSize * elementCount);

        return result >= 0 ? static_cast<size_t>(result) / elementSize : 0;
    }

    size_t Write(const void* data, size_t elementSize, size_t elementCount) override
    {
        // This is not meant for writing.
        assert(false);
        throw std::runtime_error("This is not a stream for output!");
    }

    void Skip(int64_t amount) override
    {
        while (amount > 0)
        {
            char temp[1024];
            const size_t toRead = amount > sizeof temp ? sizeof temp : static_cast<size_t>(amount);
            unzReadCurrentFile(m_container, temp, toRead);
            amount -= toRead;
        }
    }

    size_t Printf(const char* fmt, ...) override
    {
        // This is not meant for writing.
        assert(false);
        throw std::runtime_error("This is not a stream for output!");
    }

    int64_t Pos() override
    {
        return unztell(m_container);
    }

    void Goto(const int64_t pos) override
    {
        const auto current = Pos();

        if(pos > current)
        {
            Skip(pos - current);
        }
        else if(pos == current)
        {
            // This is fine.
        }
        else
        {
            // Unsupported for zip entries
            assert(false);
            throw std::runtime_error("Going backwards is not supported in IWD files!");
        }
    }

    void GotoEnd() override
    {
        Goto(m_size);
    }

    void Close() override
    {
        unzCloseCurrentFile(m_container);
        m_open = false;

        m_parent->OnIWDFileClose();
    }
};

class IWD::Impl : public ISearchPath, public IObjContainer, public IWDFile::IParent
{
    class IWDEntry
    {
    public:
        size_t m_size{};
        unz_file_pos m_file_pos{};
    };

    std::string m_path;
    FileAPI::IFile* m_file;
    unzFile m_unz_file;

    IWDFile* m_last_file;

    std::map<std::string, IWDEntry> m_entry_map;

public:
    Impl(std::string path, FileAPI::IFile* file)
    {
        m_unz_file = nullptr;
        m_path = std::move(path);
        m_file = file;
        m_last_file = nullptr;
    }

    ~Impl() override
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

    Impl(const Impl& other) = delete;
    Impl(Impl&& other) noexcept = default;
    Impl& operator=(const Impl& other) = delete;
    Impl& operator=(Impl&& other) noexcept = default;

    bool Initialize()
    {
        auto ioFunctions = FileToZlibWrapper::CreateFunctions32ForFile(m_file);
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
                entry.m_size = info.uncompressed_size;
                unzGetFilePos(m_unz_file, &entry.m_file_pos);
                m_entry_map.emplace(std::move(fileName), entry);
            }

            ret = unzGoToNextFile(m_unz_file);
        }

        if (ObjLoading::Configuration.Verbose)
        {
            printf("Loaded IWD \"%s\" with %u entries\n", m_path.c_str(), m_entry_map.size());
        }

        return true;
    }

    FileAPI::IFile* Open(const std::string& fileName) override
    {
        if (m_unz_file == nullptr)
        {
            return nullptr;
        }

        std::string iwdFilename = fileName;
        std::replace(iwdFilename.begin(), iwdFilename.end(), '\\', '/');

        const auto iwdEntry = m_entry_map.find(iwdFilename);

        if (iwdEntry != m_entry_map.end())
        {
            if(m_last_file != nullptr)
            {
                throw std::runtime_error("Trying to open new IWD file while last one was not yet closed.");
            }

            auto pos = iwdEntry->second.m_file_pos;
            unzGoToFilePos(m_unz_file, &pos);

            if (unzOpenCurrentFile(m_unz_file) == UNZ_OK)
            {
                m_last_file = new IWDFile(this, m_unz_file, iwdEntry->second.m_size);
            }

            return m_last_file;
        }

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
        if(options.m_disk_files_only)
        {
            return;
        }

        for(auto& [entryName, entry] : m_entry_map)
        {
            std::filesystem::path entryPath(entryName);

            if(!options.m_should_include_subdirectories && entryPath.has_parent_path())
                continue;

            if(options.m_filter_extensions && options.m_extension != entryPath.extension().string())
                continue;

            callback(entryName);
        }
    }

    void OnIWDFileClose() override
    {
        m_last_file = nullptr;
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
