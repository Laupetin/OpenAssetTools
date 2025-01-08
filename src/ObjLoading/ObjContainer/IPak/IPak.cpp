#include "IPak.h"

#include "IPakStreamManager.h"
#include "ObjContainer/IPak/IPakTypes.h"
#include "zlib.h"

#include <filesystem>
#include <format>
#include <iostream>
#include <memory>
#include <vector>

namespace fs = std::filesystem;

ObjContainerRepository<IIPak, Zone> IIPak::Repository;

namespace
{
    std::uint32_t R_HashString(const char* str, std::uint32_t hash)
    {
        for (const auto* pos = str; *pos; pos++)
        {
            hash = 33 * hash ^ (*pos | 0x20);
        }

        return hash;
    }
} // namespace

namespace
{
    class IPak final : public IIPak
    {
    public:
        IPak(std::string path, std::unique_ptr<std::istream> stream)
            : m_path(std::move(path)),
              m_stream(std::move(stream)),
              m_initialized(false),
              m_index_section(nullptr),
              m_data_section(nullptr),
              m_stream_manager(*m_stream)
        {
        }

        bool Initialize() override
        {
            if (m_initialized)
                return true;

            if (!ReadHeader())
                return false;

            m_initialized = true;
            return true;
        }

        [[nodiscard]] std::unique_ptr<iobjstream> GetEntryStream(const Hash nameHash, const Hash dataHash) const override
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

        std::string GetName() override
        {
            return fs::path(m_path).filename().replace_extension("").string();
        }

    private:
        bool ReadIndexSection()
        {
            m_stream->seekg(m_index_section->offset);
            IPakIndexEntry indexEntry{};

            for (unsigned itemIndex = 0; itemIndex < m_index_section->itemCount; itemIndex++)
            {
                m_stream->read(reinterpret_cast<char*>(&indexEntry), sizeof(indexEntry));
                if (m_stream->gcount() != sizeof(indexEntry))
                {
                    std::cerr << std::format("Unexpected eof when trying to load index entry {}.\n", itemIndex);
                    return false;
                }

                m_index_entries.push_back(indexEntry);
            }

            std::ranges::sort(m_index_entries,
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
                std::cerr << "Unexpected eof when trying to load section.\n";
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
                std::cerr << "Unexpected eof when trying to load header.\n";
                return false;
            }

            if (header.magic != ipak_consts::IPAK_MAGIC)
            {
                std::cerr << std::format("Invalid ipak magic '{:#x}'.\n", header.magic);
                return false;
            }

            if (header.version != ipak_consts::IPAK_VERSION)
            {
                std::cerr << std::format("Unsupported ipak version '{}'.\n", header.version);
                return false;
            }

            for (unsigned section = 0; section < header.sectionCount; section++)
            {
                if (!ReadSection())
                    return false;
            }

            if (m_index_section == nullptr)
            {
                std::cerr << "IPak does not contain an index section.\n";
                return false;
            }

            if (m_data_section == nullptr)
            {
                std::cerr << "IPak does not contain a data section.\n";
                return false;
            }

            if (!ReadIndexSection())
                return false;

            return true;
        }

        std::string m_path;
        std::unique_ptr<std::istream> m_stream;

        bool m_initialized;

        std::unique_ptr<IPakSection> m_index_section;
        std::unique_ptr<IPakSection> m_data_section;

        std::vector<IPakIndexEntry> m_index_entries;

        IPakStreamManager m_stream_manager;
    };
} // namespace

std::unique_ptr<IIPak> IIPak::Create(std::string path, std::unique_ptr<std::istream> stream)
{
    return std::make_unique<IPak>(std::move(path), std::move(stream));
}

IIPak::Hash IIPak::HashString(const std::string& str)
{
    return R_HashString(str.c_str(), 0);
}

IIPak::Hash IIPak::HashData(const void* data, const size_t dataSize)
{
    return crc32(0, static_cast<const Bytef*>(data), dataSize);
}
