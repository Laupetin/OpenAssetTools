#include "IPak.h"

#include "IPakStreamManager.h"
#include "ObjContainer/IPak/IPakTypes.h"
#include "Utils/Endianness.h"
#include "Utils/Logging/Log.h"

#include <concepts>
#include <filesystem>
#include <format>
#include <iostream>
#include <memory>
#include <vector>
#include <zlib.h>

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
              m_little_endian(true),
              m_index_section(nullptr),
              m_data_section(nullptr),
              m_stream_manager(nullptr)
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

        [[nodiscard]] std::unique_ptr<iobjstream> GetEntryStream(const IPakHash nameHash, const IPakHash dataHash) const override
        {
            const IPakIndexEntryKey wantedKey{
                {.dataHash = dataHash, .nameHash = nameHash}
            };

            for (auto& entry : m_index_entries)
            {
                if (entry.key.combinedKey == wantedKey.combinedKey)
                {
                    return m_stream_manager->OpenStream(static_cast<int64_t>(m_data_section->offset) + entry.offset, entry.size);
                }
                else if (entry.key.combinedKey > wantedKey.combinedKey)
                {
                    // The index entries are sorted so if the current entry is higher than the wanted entry we can cancel here
                    return nullptr;
                }
            }

            return nullptr;
        }

        [[nodiscard]] const std::vector<IPakIndexEntry>& GetIndexEntries() const override
        {
            return m_index_entries;
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
                    con::error("Unexpected eof when trying to load index entry {}.", itemIndex);
                    return false;
                }

                SwapBytesIfNecessary(indexEntry.key.combinedKey);
                SwapBytesIfNecessary(indexEntry.offset);
                SwapBytesIfNecessary(indexEntry.size);

                m_index_entries.emplace_back(indexEntry);
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
                con::error("Unexpected eof when trying to load section.");
                return false;
            }

            SwapBytesIfNecessary(section.type);
            SwapBytesIfNecessary(section.offset);
            SwapBytesIfNecessary(section.size);
            SwapBytesIfNecessary(section.itemCount);

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
                con::error("Unexpected eof when trying to load header.");
                return false;
            }

            if (header.magic == ipak_consts::IPAK_MAGIC_LITTLE_ENDIAN)
            {
                m_little_endian = true;
                m_stream_manager = IPakStreamManager::Create(*m_stream, true);
            }
            else if (header.magic == ipak_consts::IPAK_MAGIC_BIG_ENDIAN)
            {
                m_little_endian = false;
                m_stream_manager = IPakStreamManager::Create(*m_stream, false);
            }
            else
            {
                con::error("Invalid ipak magic '{:#x}'.", header.magic);
                return false;
            }

            SwapBytesIfNecessary(header.version);
            if (header.version != ipak_consts::IPAK_VERSION)
            {
                con::error("Unsupported ipak version '{}'.", header.version);
                return false;
            }

            SwapBytesIfNecessary(header.size);
            SwapBytesIfNecessary(header.sectionCount);
            for (unsigned section = 0; section < header.sectionCount; section++)
            {
                if (!ReadSection())
                    return false;
            }

            if (m_index_section == nullptr)
            {
                con::error("IPak does not contain an index section.");
                return false;
            }

            if (m_data_section == nullptr)
            {
                con::error("IPak does not contain a data section.");
                return false;
            }

            if (!ReadIndexSection())
                return false;

            return true;
        }

        template<std::integral T> void SwapBytesIfNecessary(T& value)
        {
            if (m_little_endian)
                value = endianness::FromLittleEndian(value);
            else
                value = endianness::FromBigEndian(value);
        }

        std::string m_path;
        std::unique_ptr<std::istream> m_stream;

        bool m_initialized;
        bool m_little_endian;

        std::unique_ptr<IPakSection> m_index_section;
        std::unique_ptr<IPakSection> m_data_section;

        std::vector<IPakIndexEntry> m_index_entries;

        std::unique_ptr<IPakStreamManager> m_stream_manager;
    };
} // namespace

std::unique_ptr<IIPak> IIPak::Create(std::string path, std::unique_ptr<std::istream> stream)
{
    return std::make_unique<IPak>(std::move(path), std::move(stream));
}

IPakHash IIPak::HashString(const std::string& str)
{
    return R_HashString(str.c_str(), 0);
}

IPakHash IIPak::HashData(const void* data, const size_t dataSize)
{
    return crc32(0, static_cast<const Bytef*>(data), static_cast<unsigned>(dataSize));
}
