#include "IPak.h"
#include "zlib.h"
#include "Exception/IPakLoadException.h"
#include "ObjContainer/IPak/IPakTypes.h"
#include "Utils/PathUtils.h"
#include "IPakStreamManager.h"

#include <sstream>
#include <vector>

ObjContainerRepository<IPak, Zone> IPak::Repository;

class IPak::Impl : public ObjContainerReferenceable
{
    static const uint32_t MAGIC = 'IPAK';
    static const uint32_t VERSION = 0x50000;

    std::string m_path;
    FileAPI::IFile* m_file;

    bool m_initialized;

    IPakSection* m_index_section;
    IPakSection* m_data_section;

    std::vector<IPakIndexEntry> m_index_entries;

    IPakStreamManager m_stream_manager;

    static uint32_t R_HashString(const char* str, uint32_t hash)
    {
        for (const char* pos = str; *pos; pos++)
        {
            hash = 33 * hash ^ (*pos | 0x20);
        }

        return hash;
    }

    bool ReadIndexSection()
    {
        m_file->Goto(m_index_section->offset);
        IPakIndexEntry indexEntry{};

        for (unsigned itemIndex = 0; itemIndex < m_index_section->itemCount; itemIndex++)
        {
            if (m_file->Read(&indexEntry, sizeof indexEntry, 1) != 1)
            {
                printf("Unexpected eof when trying to load index entry %u.\n", itemIndex);
                return false;
            }

            m_index_entries.push_back(indexEntry);
        }

        std::sort(m_index_entries.begin(), m_index_entries.end(),
                  [](const IPakIndexEntry& entry1, const IPakIndexEntry& entry2)
                  {
                      return entry1.key.combinedKey < entry2.key.combinedKey;
                  });

        return true;
    }

    bool ReadSection()
    {
        IPakSection section{};

        if (m_file->Read(&section, sizeof section, 1) != 1)
        {
            printf("Unexpected eof when trying to load section.\n");
            return false;
        }

        switch (section.type)
        {
        case 1:
            m_index_section = new IPakSection(section);
            break;

        case 2:
            m_data_section = new IPakSection(section);
            break;

        default:
            break;
        }

        return true;
    }

    bool ReadHeader()
    {
        IPakHeader header{};

        if (m_file->Read(&header, sizeof header, 1) != 1)
        {
            printf("Unexpected eof when trying to load header.\n");
            return false;
        }

        if (header.magic != MAGIC)
        {
            printf("Invalid ipak magic '0x%x'.\n", header.magic);
            return false;
        }

        if (header.version != VERSION)
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
    Impl(std::string path, FileAPI::IFile* file)
        : m_stream_manager(file)
    {
        m_path = std::move(path);
        m_file = file;
        m_initialized = false;
        m_index_section = nullptr;
        m_data_section = nullptr;
    }

    ~Impl()
    {
        delete m_index_section;
        m_index_section = nullptr;

        delete m_data_section;
        m_data_section = nullptr;
    }

    std::string GetName() override
    {
        return utils::Path::GetFilename(m_path);
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

    FileAPI::IFile* GetEntryData(const Hash nameHash, const Hash dataHash)
    {
        IPakIndexEntryKey wantedKey{};
        wantedKey.nameHash = nameHash;
        wantedKey.dataHash = dataHash;

        for (auto& entry : m_index_entries)
        {
            if (entry.key.combinedKey == wantedKey.combinedKey)
            {
                return m_stream_manager.OpenStream(m_data_section->offset + entry.offset, entry.size);
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

IPak::IPak(std::string path, FileAPI::IFile* file)
{
    m_impl = new Impl(std::move(path), file);
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

bool IPak::Initialize() const
{
    return m_impl->Initialize();
}

FileAPI::IFile* IPak::GetEntryStream(const Hash nameHash, const Hash dataHash) const
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
