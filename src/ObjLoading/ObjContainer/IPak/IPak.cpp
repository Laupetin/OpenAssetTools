#include "IPak.h"

#include "Exception/IPakLoadException.h"
#include "IPakStreamManager.h"
#include "ObjContainer/IPak/IPakTypes.h"
#include "Utils/FileUtils.h"
#include "zlib.h"

#include <filesystem>
#include <memory>
#include <sstream>
#include <vector>

namespace fs = std::filesystem;

ObjContainerRepository<IPak, Zone> IPak::Repository;

class IPak::Impl : public ObjContainerReferenceable
{
    std::string m_path;
    std::unique_ptr<std::istream> m_stream;

    bool m_initialized;

    std::unique_ptr<IPakSection> m_index_section;
    std::unique_ptr<IPakSection> m_data_section;

    std::vector<IPakIndexEntry> m_index_entries;

    IPakStreamManager m_stream_manager;

    static uint32_t R_HashString(const char* str, uint32_t hash)
    {
        for (const auto* pos = str; *pos; pos++)
        {
            hash = 33 * hash ^ (*pos | 0x20);
        }

        return hash;
    }

    bool ReadIndexSection()
    {
        m_stream->seekg(m_index_section->offset);
        IPakIndexEntry indexEntry{};

        for (unsigned itemIndex = 0; itemIndex < m_index_section->itemCount; itemIndex++)
        {
            m_stream->read(reinterpret_cast<char*>(&indexEntry), sizeof(indexEntry));
            if (m_stream->gcount() != sizeof(indexEntry))
            {
                printf("Unexpected eof when trying to load index entry %u.\n", itemIndex);
                return false;
            }

            m_index_entries.push_back(indexEntry);
        }

        std::sort(m_index_entries.begin(),
                  m_index_entries.end(),
                  [](const IPakIndexEntry& entry1, const IPakIndexEntry& entry2)
                  {
                      return entry1.key.combinedKey < entry2.key.combinedKey;
                  });

        return true;
    }

    bool ReadSection()
    {
        IPakSection section{};

        m_stream->read(reinterpret_cast<char*>(&section), sizeof(section));
        if (m_stream->gcount() != sizeof(section))
        {
            printf("Unexpected eof when trying to load section.\n");
            return false;
        }

        switch (section.type)
        {
        case ipak_consts::IPAK_INDEX_SECTION:
            m_index_section = std::make_unique<IPakSection>(section);
            break;

        case ipak_consts::IPAK_DATA_SECTION:
            m_data_section = std::make_unique<IPakSection>(section);
            break;

        default:
            break;
        }

        return true;
    }

    bool ReadHeader()
    {
        IPakHeader header{};

        m_stream->read(reinterpret_cast<char*>(&header), sizeof(header));
        if (m_stream->gcount() != sizeof(header))
        {
            printf("Unexpected eof when trying to load header.\n");
            return false;
        }

        if (header.magic != ipak_consts::IPAK_MAGIC)
        {
            printf("Invalid ipak magic '0x%x'.\n", header.magic);
            return false;
        }

        if (header.version != ipak_consts::IPAK_VERSION)
        {
            printf("Unsupported ipak version '%u'.\n", header.version);
            return false;
        }

        for (unsigned section = 0; section < header.sectionCount; section++)
        {
            if (!ReadSection())
                return false;
        }

        if (m_index_section == nullptr)
        {
            printf("IPak does not contain an index section.\n");
            return false;
        }

        if (m_data_section == nullptr)
        {
            printf("IPak does not contain a data section.\n");
            return false;
        }

        if (!ReadIndexSection())
            return false;

        return true;
    }

public:
    Impl(std::string path, std::unique_ptr<std::istream> stream)
        : m_path(std::move(path)),
          m_stream(std::move(stream)),
          m_initialized(false),
          m_index_section(nullptr),
          m_data_section(nullptr),
          m_stream_manager(*m_stream)
    {
    }

    ~Impl() override = default;

    std::string GetName() override
    {
        return fs::path(m_path).filename().replace_extension("").string();
    }

    bool Initialize()
    {
        if (m_initialized)
            return true;

        if (!ReadHeader())
            return false;

        m_initialized = true;
        return true;
    }

    std::unique_ptr<iobjstream> GetEntryData(const Hash nameHash, const Hash dataHash)
    {
        IPakIndexEntryKey wantedKey{};
        wantedKey.nameHash = nameHash;
        wantedKey.dataHash = dataHash;

        for (auto& entry : m_index_entries)
        {
            if (entry.key.combinedKey == wantedKey.combinedKey)
            {
                return m_stream_manager.OpenStream(static_cast<int64_t>(m_data_section->offset) + entry.offset, entry.size);
            }
            else if (entry.key.combinedKey > wantedKey.combinedKey)
            {
                // The index entries are sorted so if the current entry is higher than the wanted entry we can cancel here
                return nullptr;
            }
        }

        return nullptr;
    }

    static Hash HashString(const std::string& str)
    {
        return R_HashString(str.c_str(), 0);
    }

    static Hash HashData(const void* data, const size_t dataSize)
    {
        return crc32(0, static_cast<const Bytef*>(data), dataSize);
    }
};

IPak::IPak(std::string path, std::unique_ptr<std::istream> stream)
{
    m_impl = new Impl(std::move(path), std::move(stream));
}

IPak::~IPak()
{
    delete m_impl;
    m_impl = nullptr;
}

std::string IPak::GetName()
{
    return m_impl->GetName();
}

bool IPak::Initialize()
{
    return m_impl->Initialize();
}

std::unique_ptr<iobjstream> IPak::GetEntryStream(const Hash nameHash, const Hash dataHash) const
{
    return m_impl->GetEntryData(nameHash, dataHash);
}

IPak::Hash IPak::HashString(const std::string& str)
{
    return Impl::HashString(str);
}

IPak::Hash IPak::HashData(const void* data, const size_t dataSize)
{
    return Impl::HashData(data, dataSize);
}
