#include "SoundBank.h"

#include "Utils/ObjStream.h"
#include "zlib.h"

#include <cstring>
#include <memory>
#include <sstream>
#include <vector>

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
            if (off > m_size)
                return pos_type(-1);

            return seekpos(m_size - off, mode);
        }

        return seekpos(m_offset + off, mode);
    }

    pos_type seekpos(const pos_type pos, std::ios_base::openmode mode) override
    {
        if (pos < 0 || pos >= m_size)
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

    _NODISCARD bool is_open() const override
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

SoundBankEntryInputStream::SoundBankEntryInputStream(std::unique_ptr<std::istream> stream, const SoundAssetBankEntry& entry)
    : m_stream(std::move(stream)),
      m_entry(entry)
{
}

bool SoundBankEntryInputStream::IsOpen() const
{
    return static_cast<bool>(m_stream);
}

bool SoundBank::ReadHeader()
{
    m_stream->read(reinterpret_cast<char*>(&m_header), sizeof(m_header));
    if (m_stream->gcount() != sizeof(m_header))
    {
        printf("Unexpected eof when trying to load sndbank header.\n");
        return false;
    }

    if (m_header.magic != sndbank_consts::MAGIC)
    {
        std::cout << "Invalid sndbank magic 0x" << std::hex << m_header.magic << std::endl;
        return false;
    }

    if (m_header.version != sndbank_consts::VERSION)
    {
        std::cout << "Unsupported sndbank version " << m_header.version << " (should be " << sndbank_consts::VERSION << ")" << std::endl;
        return false;
    }

    if (m_header.entrySize != sizeof(SoundAssetBankEntry))
    {
        std::cout << "Invalid sndbank entry size 0x" << std::hex << m_header.entrySize << " (should be 0x" << std::hex << sizeof(SoundAssetBankEntry) << ")"
                  << std::endl;
        return false;
    }

    if (m_header.fileSize != m_file_size)
    {
        std::cout << "Invalid sndbank " << m_file_size << " (header expects " << m_header.fileSize << ")" << std::endl;
        return false;
    }

    if (m_header.entryCount
        && (m_header.entryOffset <= 0 || m_header.entryOffset + static_cast<int64_t>(sizeof(SoundAssetBankEntry) * m_header.entryCount) > m_file_size))
    {
        std::cout << "Invalid sndbank entry offset " << m_header.entryOffset << " (filesize is " << m_file_size << ")" << std::endl;
        return false;
    }

    if (m_header.checksumOffset <= 0 || m_header.checksumOffset + static_cast<int64_t>(sizeof(SoundAssetBankChecksum) * m_header.entryCount) > m_file_size)
    {
        std::cout << "Invalid sndbank checksum offset " << m_header.checksumOffset << " (filesize is " << m_file_size << ")" << std::endl;
        return false;
    }

    if (m_header.dependencyCount * m_header.dependencySize > sizeof(SoundAssetBankHeader::dependencies))
    {
        std::cout << "Invalid sndbank dependency sizes (count is " << m_header.dependencyCount << "; size is " << m_header.dependencySize << ")" << std::endl;
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
            std::cout << "Failed to read sound bank entry at index " << i << std::endl;
            return false;
        }

        if (entry.offset == 0 || entry.offset + entry.size >= m_file_size)
        {
            std::cout << "Invalid sound bank entry data offset " << entry.offset << " (filesize is " << m_header.fileSize << ")" << std::endl;
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
            std::cout << "Failed to read sound bank checksum at index " << i << std::endl;
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
