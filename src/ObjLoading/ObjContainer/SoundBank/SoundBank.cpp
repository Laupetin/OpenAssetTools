#include "SoundBank.h"

#include "Utils/Logging/Log.h"

#include <cstring>
#include <format>
#include <memory>
#include <sstream>
#include <vector>
#include <zlib.h>

ObjContainerRepository<SoundBank, Zone> SoundBank::Repository;

class SoundBankInputBuffer final : public objbuf
{
    std::istream& m_stream;
    int64_t m_base_offset;
    size_t m_size;
    size_t m_offset;
    bool m_open;

protected:
    std::streamsize showmanyc() override
    {
        return m_size - m_offset;
    }

    int_type underflow() override
    {
        if (m_offset >= m_size)
            return EOF;

        return m_stream.peek();
    }

    int_type uflow() override
    {
        if (m_offset >= m_size)
            return EOF;

        m_offset++;
        return m_stream.get();
    }

    std::streamsize xsgetn(char* ptr, std::streamsize count) override
    {
        if (m_offset + count > m_size)
            count = m_size - m_offset;

        if (count > 0)
        {
            m_stream.read(ptr, count);

            const auto readSize = m_stream.gcount();
            m_offset += static_cast<size_t>(readSize);
            return readSize;
        }

        return 0;
    }

    pos_type seekoff(const off_type off, const std::ios_base::seekdir dir, const std::ios_base::openmode mode) override
    {
        if (dir == std::ios_base::beg)
        {
            return seekpos(off, mode);
        }

        if (dir == std::ios_base::end)
        {
            if (off > static_cast<off_type>(m_size))
                return pos_type(-1);

            return seekpos(static_cast<off_type>(m_size) - off, mode);
        }

        return seekpos(static_cast<pos_type>(m_offset) + off, mode);
    }

    pos_type seekpos(const pos_type pos, std::ios_base::openmode mode) override
    {
        if (pos < 0 || pos >= static_cast<pos_type>(m_size))
            return pos_type(-1);

        m_stream.seekg(m_base_offset + pos);
        m_offset = static_cast<size_t>(pos);
        return pos;
    }

public:
    SoundBankInputBuffer(std::istream& stream, const int64_t baseOffset, const size_t size)
        : m_stream(stream),
          m_base_offset(baseOffset),
          m_size(size),
          m_offset(0),
          m_open(true)
    {
    }

    [[nodiscard]] bool is_open() const override
    {
        return m_open;
    }

    bool close() override
    {
        const auto result = m_open;
        m_open = false;
        return result;
    }
};

SoundBankEntryInputStream::SoundBankEntryInputStream()
    : m_entry{}
{
}

SoundBankEntryInputStream::SoundBankEntryInputStream(std::unique_ptr<std::istream> stream, SoundAssetBankEntry entry)
    : m_stream(std::move(stream)),
      m_entry(std::move(entry))
{
}

bool SoundBankEntryInputStream::IsOpen() const
{
    return m_stream != nullptr;
}

bool SoundBank::ReadHeader()
{
    m_stream->read(reinterpret_cast<char*>(&m_header), sizeof(m_header));
    if (m_stream->gcount() != sizeof(m_header))
    {
        con::error("Unexpected eof when trying to load sndbank header.");
        return false;
    }

    if (m_header.magic != MAGIC)
    {
        con::error("Invalid sndbank magic 0x{:x}", m_header.magic);
        return false;
    }

    if (m_header.version != VERSION)
    {
        con::error("Unsupported sndbank version {} (should be {})", m_header.version, VERSION);
        return false;
    }

    if (m_header.entrySize != sizeof(SoundAssetBankEntry))
    {
        con::error("Invalid sndbank entry size 0x{:x} (should be 0x{:x})", m_header.entrySize, sizeof(SoundAssetBankEntry));
        return false;
    }

    if (m_header.fileSize != m_file_size)
    {
        con::error("Invalid sndbank {} (header expects {})", m_file_size, m_header.fileSize);
        return false;
    }

    if (m_header.entryCount
        && (m_header.entryOffset <= 0 || m_header.entryOffset + sizeof(SoundAssetBankEntry) * m_header.entryCount > static_cast<size_t>(m_file_size)))
    {
        con::error("Invalid sndbank entry offset {} (filesize is {})", m_header.entryOffset, m_file_size);
        return false;
    }

    if (m_header.checksumOffset <= 0 || m_header.checksumOffset + sizeof(SoundAssetBankChecksum) * m_header.entryCount > static_cast<size_t>(m_file_size))
    {
        con::error("Invalid sndbank checksum offset {} (filesize is {})", m_header.checksumOffset, m_file_size);
        return false;
    }

    if (m_header.dependencyCount * m_header.dependencySize > sizeof(SoundAssetBankHeader::dependencies))
    {
        con::error("Invalid sndbank dependency sizes (count is {}; size is {})", m_header.dependencyCount, m_header.dependencySize);
        return false;
    }

    for (auto i = 0u; i < m_header.dependencyCount; i++)
    {
        const auto dependencyLen = strnlen(&m_header.dependencies[i * m_header.dependencySize], m_header.dependencySize);
        std::string dependencyName(&m_header.dependencies[i * m_header.dependencySize], dependencyLen);

        if (dependencyName.empty())
            continue;

        m_dependencies.emplace_back(std::move(dependencyName));
    }

    return true;
}

bool SoundBank::ReadEntries()
{
    m_stream->seekg(m_header.entryOffset);

    for (auto i = 0u; i < m_header.entryCount; i++)
    {
        SoundAssetBankEntry entry{};
        m_stream->read(reinterpret_cast<char*>(&entry), sizeof(entry));

        if (m_stream->gcount() != sizeof(entry))
        {
            con::error("Failed to read sound bank entry at index {}", i);
            return false;
        }

        if (entry.offset == 0 || entry.offset + entry.size >= m_file_size)
        {
            con::error("Invalid sound bank entry data offset {} (filesize is {})", entry.offset, m_header.fileSize);
            return false;
        }

        m_entries.push_back(entry);
        m_entries_by_id.emplace(std::make_pair(entry.id, i));
    }

    return true;
}

bool SoundBank::ReadChecksums()
{
    m_stream->seekg(m_header.entryOffset);

    for (auto i = 0u; i < m_header.entryCount; i++)
    {
        SoundAssetBankChecksum checksum{};
        m_stream->read(reinterpret_cast<char*>(&checksum), sizeof(checksum));

        if (m_stream->gcount() != sizeof(checksum))
        {
            con::error("Failed to read sound bank checksum at index {}", i);
            return false;
        }

        m_checksums.push_back(checksum);
    }

    return true;
}

std::string SoundBank::GetFileNameForDefinition(const bool streamed, const char* zone, const char* language)
{
    std::ostringstream str;

    assert(zone != nullptr);

    if (zone)
        str << zone;

    if (language)
        str << "." << language;

    if (streamed)
        str << ".sabs";
    else
        str << ".sabl";

    return str.str();
}

SoundBank::SoundBank(std::string fileName, std::unique_ptr<std::istream> stream, const int64_t fileSize)
    : m_file_name(std::move(fileName)),
      m_stream(std::move(stream)),
      m_file_size(fileSize),
      m_initialized(false),
      m_header{}
{
}

std::string SoundBank::GetName()
{
    return m_file_name;
}

bool SoundBank::Initialize()
{
    if (m_initialized)
        return true;

    if (!ReadHeader() || !ReadEntries() || !ReadChecksums())
        return false;

    m_initialized = true;
    return true;
}

const std::vector<std::string>& SoundBank::GetDependencies() const
{
    return m_dependencies;
}

bool SoundBank::VerifyChecksum(const SoundAssetBankChecksum& checksum) const
{
    return m_initialized && memcmp(checksum.checksumBytes, m_header.checksumChecksum.checksumBytes, sizeof(SoundAssetBankChecksum)) == 0;
}

SoundBankEntryInputStream SoundBank::GetEntryStream(const unsigned id) const
{
    const auto foundEntry = m_entries_by_id.find(id);

    if (foundEntry != m_entries_by_id.end())
    {
        const auto& entry = m_entries[foundEntry->second];

        m_stream->seekg(entry.offset);

        return SoundBankEntryInputStream(std::make_unique<iobjstream>(std::make_unique<SoundBankInputBuffer>(*m_stream, entry.offset, entry.size)), entry);
    }

    return SoundBankEntryInputStream();
}
