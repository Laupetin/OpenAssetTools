#include "IWD.h"

#include "ObjLoading.h"
#include "Utils/FileToZlibWrapper.h"

#include <unzip.h>
#include <filesystem>
#include <cassert>
#include <map>
#include <fstream>
#include <memory>

namespace fs = std::filesystem;

ObjContainerRepository<IWD, ISearchPath> IWD::Repository;

class IWDFile final : public objbuf
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
    int64_t m_size;
    unzFile m_container;
    bool m_peeked;
    int_type m_peek_symbol;

public:
    IWDFile(IParent* parent, const unzFile container, const int64_t size)
        : m_parent(parent),
          m_open(true),
          m_size(size),
          m_container(container),
          m_peeked(false),
          m_peek_symbol(0)
    {
    }

    ~IWDFile() override
    {
        if (m_open)
        {
            close();
        }
    }

protected:
    int_type underflow() override
    {
        if (m_peeked)
            return m_peek_symbol;

        const auto result = unzReadCurrentFile(m_container, &m_peek_symbol, 1u);

        if (result >= 0)
        {
            m_peeked = true;
            return static_cast<int_type>(m_peek_symbol);
        }

        return EOF;
    }

    int_type uflow() override
    {
        if (m_peeked)
        {
            m_peeked = false;
            return m_peek_symbol;
        }

        const auto result = unzReadCurrentFile(m_container, &m_peek_symbol, 1u);
        return result >= 0 ? static_cast<int_type>(m_peek_symbol) : EOF;
    }

    std::streamsize xsgetn(char* ptr, std::streamsize count) override
    {
        if (m_peeked && count >= 1)
        {
            *ptr = static_cast<char>(m_peek_symbol);
            ptr++;
            count--;
        }

        const auto result = unzReadCurrentFile(m_container, ptr, static_cast<unsigned>(count));

        return result >= 0 ? static_cast<std::streamsize>(result) : 0;
    }

    pos_type seekoff(const off_type off, const std::ios_base::seekdir dir, const std::ios_base::openmode mode) override
    {
        const auto currentPos = unztell64(m_container);

        pos_type targetPos;
        if (dir == std::ios_base::beg)
        {
            targetPos = off;
        }
        else if (dir == std::ios_base::cur)
        {
            targetPos = currentPos + off;
        }
        else
        {
            targetPos = m_size - off;
        }

        return seekpos(targetPos, mode);
    }

    pos_type seekpos(const pos_type pos, const std::ios_base::openmode mode) override
    {
        const auto currentPos = unztell64(m_container);

        if (static_cast<pos_type>(currentPos) < pos)
        {
            auto skipAmount = pos - static_cast<pos_type>(currentPos);
            while (skipAmount > 0)
            {
                char temp[1024];
                const auto toRead = skipAmount > sizeof temp ? sizeof temp : static_cast<size_t>(skipAmount);
                unzReadCurrentFile(m_container, temp, toRead);
                skipAmount -= toRead;
            }

            return pos;
        }

        if (currentPos == pos)
        {
            // This is fine
            return currentPos;
        }

        return std::streampos(-1);
    }

public:
    _NODISCARD bool is_open() const override
    {
        return m_open;
    }

    bool close() override
    {
        unzCloseCurrentFile(m_container);
        m_open = false;

        m_parent->OnIWDFileClose();

        return true;
    }
};

class IWD::Impl : public ISearchPath, public IObjContainer, public IWDFile::IParent
{
    class IWDEntry
    {
    public:
        int64_t m_size{};
        unz_file_pos m_file_pos{};
    };

    std::string m_path;
    std::unique_ptr<std::istream> m_stream;
    unzFile m_unz_file;

    IWDFile* m_last_file;

    std::map<std::string, IWDEntry> m_entry_map;

public:
    Impl(std::string path, std::unique_ptr<std::istream> stream)
        : m_path(std::move(path)),
          m_stream(std::move(stream)),
          m_unz_file(nullptr),
          m_last_file(nullptr)
    {
    }

    ~Impl() override
    {
        if (m_unz_file != nullptr)
        {
            unzClose(m_unz_file);
            m_unz_file = nullptr;
        }
    }

    Impl(const Impl& other) = delete;
    Impl(Impl&& other) noexcept = default;
    Impl& operator=(const Impl& other) = delete;
    Impl& operator=(Impl&& other) noexcept = default;

    bool Initialize()
    {
        auto ioFunctions = FileToZlibWrapper::CreateFunctions32ForFile(m_stream.get());
        m_unz_file = unzOpen2("", &ioFunctions);

        if (m_unz_file == nullptr)
        {
            printf("Could not open IWD \"%s\"\n", m_path.c_str());
            return false;
        }

        auto ret = unzGoToFirstFile(m_unz_file);
        while (ret == Z_OK)
        {
            unz_file_info64 info;
            char fileNameBuffer[256];
            unzGetCurrentFileInfo64(m_unz_file, &info, fileNameBuffer, sizeof fileNameBuffer, nullptr, 0, nullptr, 0);

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

    std::unique_ptr<std::istream> Open(const std::string& fileName) override
    {
        if (m_unz_file == nullptr)
        {
            return nullptr;
        }

        auto iwdFilename = fileName;
        std::replace(iwdFilename.begin(), iwdFilename.end(), '\\', '/');

        const auto iwdEntry = m_entry_map.find(iwdFilename);

        if (iwdEntry != m_entry_map.end())
        {
            if (m_last_file != nullptr)
            {
                throw std::runtime_error("Trying to open new IWD file while last one was not yet closed.");
            }

            auto pos = iwdEntry->second.m_file_pos;
            unzGoToFilePos(m_unz_file, &pos);

            if (unzOpenCurrentFile(m_unz_file) == UNZ_OK)
            {
                auto result = std::make_unique<IWDFile>(this, m_unz_file, iwdEntry->second.m_size);
                m_last_file = result.get();
                return std::make_unique<iobjstream>(std::move(result));
            }

            return nullptr;
        }

        return nullptr;
    }

    std::string GetPath() override
    {
        return m_path;
    }

    std::string GetName() override
    {
        return fs::path(m_path).filename().string();
    }

    void Find(const SearchPathSearchOptions& options, const std::function<void(const std::string&)>& callback) override
    {
        if (options.m_disk_files_only)
        {
            return;
        }

        for (auto& [entryName, entry] : m_entry_map)
        {
            std::filesystem::path entryPath(entryName);

            if (!options.m_should_include_subdirectories && entryPath.has_parent_path())
                continue;

            if (options.m_filter_extensions && options.m_extension != entryPath.extension().string())
                continue;

            callback(entryName);
        }
    }

    void OnIWDFileClose() override
    {
        m_last_file = nullptr;
    }
};

IWD::IWD(std::string path, std::unique_ptr<std::istream> stream)
{
    m_impl = new Impl(std::move(path), std::move(stream));
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

bool IWD::Initialize()
{
    return m_impl->Initialize();
}

std::unique_ptr<std::istream> IWD::Open(const std::string& fileName)
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
