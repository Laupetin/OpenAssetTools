#include "IWD.h"

#include "ObjLoading.h"
#include "Utils/FileToZlibWrapper.h"

#include <algorithm>
#include <cassert>
#include <filesystem>
#include <format>
#include <fstream>
#include <map>
#include <memory>
#include <unzip.h>

namespace fs = std::filesystem;

namespace
{
    class IwdFile final : public objbuf
    {
    public:
        class IParent
        {
        public:
            IParent() = default;
            virtual ~IParent() = default;
            IParent(const IParent& other) = default;
            IParent(IParent&& other) noexcept = default;
            IParent& operator=(const IParent& other) = default;
            IParent& operator=(IParent&& other) noexcept = default;

            virtual void OnIwdFileClose() = 0;
        };

        IwdFile(IParent* parent, const unzFile container, const int64_t size)
            : m_parent(parent),
              m_open(true),
              m_size(size),
              m_container(container),
              m_peeked(false),
              m_peek_symbol(0)
        {
        }

        ~IwdFile() override
        {
            if (m_open)
            {
                close();
            }
        }

        IwdFile(const IwdFile& other) = default;
        IwdFile(IwdFile&& other) noexcept = default;
        IwdFile& operator=(const IwdFile& other) = default;
        IwdFile& operator=(IwdFile&& other) noexcept = default;

        _NODISCARD bool is_open() const override
        {
            return m_open;
        }

        bool close() override
        {
            unzCloseCurrentFile(m_container);
            m_open = false;

            m_parent->OnIwdFileClose();

            return true;
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
                return m_peek_symbol;
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
            return result >= 0 ? m_peek_symbol : EOF;
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
                targetPos = static_cast<pos_type>(currentPos) + off;
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
                    const auto toRead = skipAmount > sizeof(temp) ? sizeof(temp) : static_cast<size_t>(skipAmount);
                    unzReadCurrentFile(m_container, temp, toRead);
                    skipAmount -= toRead;
                }

                return pos;
            }

            if (currentPos == pos)
                return currentPos;

            return std::streampos(-1);
        }

    private:
        IParent* m_parent;
        bool m_open;
        int64_t m_size;
        unzFile m_container;
        bool m_peeked;
        int_type m_peek_symbol;
    };

    struct IwdEntry
    {
        int64_t m_size;
        unz_file_pos m_file_pos;

        explicit IwdEntry(const int64_t size)
            : m_size(size),
              m_file_pos{}
        {
        }
    };

    class Iwd final : public ISearchPath, public IwdFile::IParent
    {
    public:
        Iwd(std::string path, std::ifstream stream)
            : m_path(std::move(path)),
              m_stream(std::move(stream)),
              m_unz_file{},
              m_has_open_file(false)
        {
        }

        ~Iwd() override
        {
            if (m_unz_file != nullptr)
            {
                unzClose(m_unz_file);
                m_unz_file = nullptr;
            }
        }

        Iwd(const Iwd& other) = delete;
        Iwd(Iwd&& other) noexcept = delete;
        Iwd& operator=(const Iwd& other) = delete;
        Iwd& operator=(Iwd&& other) noexcept = delete;

        /**
         * \brief Initializes the IWD container.
         * \return \c true when initialization was successful.
         */
        bool Initialize()
        {
            auto ioFunctions = FileToZlibWrapper::CreateFunctions32ForFile(&m_stream);
            m_unz_file = unzOpen2("", &ioFunctions);

            if (m_unz_file == nullptr)
            {
                std::cerr << std::format("Could not open IWD \"{}\"\n", m_path);
                return false;
            }

            auto ret = unzGoToFirstFile(m_unz_file);
            while (ret == Z_OK)
            {
                unz_file_info64 info;
                char fileNameBuffer[256];
                unzGetCurrentFileInfo64(m_unz_file, &info, fileNameBuffer, sizeof(fileNameBuffer), nullptr, 0, nullptr, 0);

                std::string fileName(fileNameBuffer);
                fs::path path(fileName);

                if (path.has_filename())
                {
                    IwdEntry entry(static_cast<std::int64_t>(info.uncompressed_size));
                    unzGetFilePos(m_unz_file, &entry.m_file_pos);
                    m_entry_map.emplace(std::move(fileName), entry);
                }

                ret = unzGoToNextFile(m_unz_file);
            }

            std::cout << std::format("Loaded IWD \"{}\" with {} entries\n", m_path, m_entry_map.size());

            return true;
        }

        SearchPathOpenFile Open(const std::string& fileName) override
        {
            if (m_unz_file == nullptr)
            {
                return SearchPathOpenFile();
            }

            auto iwdFilename = fileName;
            std::ranges::replace(iwdFilename, '\\', '/');

            const auto iwdEntry = m_entry_map.find(iwdFilename);

            if (iwdEntry != m_entry_map.end())
            {
                assert(!m_has_open_file);
                if (m_has_open_file)
                {
                    std::cerr << "Trying to open new IWD file while last one was not yet closed.\n";
                    return SearchPathOpenFile();
                }

                auto pos = iwdEntry->second.m_file_pos;
                unzGoToFilePos(m_unz_file, &pos);

                if (unzOpenCurrentFile(m_unz_file) == UNZ_OK)
                {
                    auto result = std::make_unique<IwdFile>(this, m_unz_file, iwdEntry->second.m_size);
                    m_has_open_file = true;
                    return SearchPathOpenFile(std::make_unique<iobjstream>(std::move(result)), iwdEntry->second.m_size);
                }

                return SearchPathOpenFile();
            }

            return SearchPathOpenFile();
        }

        const std::string& GetPath() override
        {
            return m_path;
        }

        void OnIwdFileClose() override
        {
            m_has_open_file = false;
        }

        void Find(const SearchPathSearchOptions& options, const std::function<void(const std::string&)>& callback) override
        {
            if (options.m_disk_files_only)
            {
                return;
            }

            for (const auto& [entryName, entry] : m_entry_map)
            {
                std::filesystem::path entryPath(entryName);

                if (!options.m_should_include_subdirectories && entryPath.has_parent_path())
                    continue;

                if (options.m_filter_extensions && options.m_extension != entryPath.extension().string())
                    continue;

                callback(entryName);
            }
        }

    private:
        std::string m_path;
        std::ifstream m_stream;
        unzFile m_unz_file;
        bool m_has_open_file;

        std::map<std::string, IwdEntry> m_entry_map;
    };
} // namespace

namespace iwd
{
    std::unique_ptr<ISearchPath> LoadFromFile(const std::string& path)
    {
        std::ifstream inputStream(path, std::ios::in | std::ios::binary);
        if (!inputStream.is_open())
            return {};

        auto iwd = std::make_unique<Iwd>(path, std::move(inputStream));
        if (!iwd->Initialize())
            return {};

        return iwd;
    }
} // namespace iwd
