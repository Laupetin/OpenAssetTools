#include "IPakCreator.h"

#include "Game/T6/CommonT6.h"
#include "Game/T6/GameT6.h"
#include "GitVersion.h"
#include "ObjContainer/IPak/IPakTypes.h"
#include "Utils/Alignment.h"

#include <algorithm>
#include <format>
#include <fstream>
#include <iostream>
#include <minilzo.h>
#include <zlib.h>

namespace
{
    constexpr auto USE_IPAK_COMPRESSION = true;

    class IPakWriter
    {
        static constexpr char BRANDING[] = "Created with OpenAssetTools " GIT_VERSION;
        static constexpr auto SECTION_COUNT = 3; // Index + Data + Branding

        inline static const std::string PAD_DATA = std::string(256, '\xA7');

    public:
        IPakWriter(std::ostream& stream, ISearchPath& searchPath, const std::vector<std::string>& images)
            : m_stream(stream),
              m_search_path(searchPath),
              m_images(images),
              m_current_offset(0),
              m_total_size(0),
              m_data_section_offset(0),
              m_data_section_size(0u),
              m_index_section_offset(0),
              m_branding_section_offset(0),
              m_file_offset(0u),
              m_chunk_buffer_window_start(0),
              m_current_block{},
              m_current_block_header_offset(0)
        {
            m_decompressed_buffer = std::make_unique<char[]>(ipak_consts::IPAK_CHUNK_SIZE);
            m_lzo_work_buffer = std::make_unique<char[]>(LZO1X_1_MEM_COMPRESS);
        }

        bool Write()
        {
            // We will write the header and sections later since they need complementary data
            GoTo(sizeof(IPakHeader) + sizeof(IPakSection) * SECTION_COUNT);
            AlignToChunk();

            if (!WriteDataSection())
                return false;

            WriteIndexSection();
            WriteBrandingSection();
            WriteFileEnding();

            WriteHeaderData();

            return true;
        }

    private:
        void GoTo(const int64_t offset)
        {
            m_stream.seekp(offset, std::ios::beg);
            m_current_offset = offset;
        }

        void Write(const void* data, const size_t dataSize)
        {
            m_stream.write(static_cast<const char*>(data), dataSize);
            m_current_offset += dataSize;
        }

        void Pad(const size_t paddingSize)
        {
            auto paddingSizeLeft = paddingSize;
            while (paddingSizeLeft > 0)
            {
                const auto writeSize = std::min(paddingSizeLeft, PAD_DATA.size());
                Write(PAD_DATA.data(), writeSize);

                paddingSizeLeft -= writeSize;
            }
        }

        void AlignToChunk()
        {
            Pad(static_cast<size_t>(utils::Align(m_current_offset, static_cast<int64_t>(ipak_consts::IPAK_CHUNK_SIZE)) - m_current_offset));
        }

        void AlignToBlockHeader()
        {
            Pad(static_cast<size_t>(utils::Align(m_current_offset, static_cast<int64_t>(sizeof(IPakDataBlockHeader))) - m_current_offset));
        }

        void WriteHeaderData()
        {
            GoTo(0);

            const IPakHeader header{ipak_consts::IPAK_MAGIC, ipak_consts::IPAK_VERSION, static_cast<uint32_t>(m_total_size), SECTION_COUNT};

            const IPakSection dataSection{
                ipak_consts::IPAK_DATA_SECTION,
                static_cast<uint32_t>(m_data_section_offset),
                static_cast<uint32_t>(m_data_section_size),
                static_cast<uint32_t>(m_index_entries.size()),
            };

            const IPakSection indexSection{
                ipak_consts::IPAK_INDEX_SECTION,
                static_cast<uint32_t>(m_index_section_offset),
                static_cast<uint32_t>(sizeof(IPakIndexEntry) * m_index_entries.size()),
                static_cast<uint32_t>(m_index_entries.size()),
            };

            const IPakSection brandingSection{
                ipak_consts::IPAK_BRANDING_SECTION,
                static_cast<uint32_t>(m_branding_section_offset),
                std::extent_v<decltype(BRANDING)>,
                1,
            };

            Write(&header, sizeof(header));
            Write(&dataSection, sizeof(dataSection));
            Write(&indexSection, sizeof(indexSection));
            Write(&brandingSection, sizeof(brandingSection));
        }

        static std::string ImageFileName(const std::string& imageName)
        {
            return std::format("images/{}.iwi", imageName);
        }

        std::unique_ptr<char[]> ReadImageDataFromSearchPath(const std::string& imageName, size_t& imageSize) const
        {
            const auto fileName = ImageFileName(imageName);

            const auto openFile = m_search_path.Open(fileName);
            if (!openFile.IsOpen())
            {
                std::cerr << std::format("Could not open image for writing to IPak \"{}\"\n", fileName);
                return nullptr;
            }

            imageSize = static_cast<size_t>(openFile.m_length);
            auto imageData = std::make_unique<char[]>(imageSize);
            openFile.m_stream->read(imageData.get(), imageSize);

            return imageData;
        }

        void FlushBlock()
        {
            if (m_current_block_header_offset > 0)
            {
                const auto previousOffset = m_current_offset;

                GoTo(m_current_block_header_offset);
                Write(&m_current_block, sizeof(m_current_block));
                GoTo(previousOffset);
            }
        }

        void FlushChunk()
        {
            FlushBlock();
            AlignToBlockHeader();

            const auto nextChunkOffset = utils::Align(m_current_offset, static_cast<int64_t>(ipak_consts::IPAK_CHUNK_SIZE));
            const auto sizeToSkip = static_cast<size_t>(nextChunkOffset - m_current_offset);

            if (sizeToSkip >= sizeof(IPakDataBlockHeader))
            {
                IPakDataBlockHeader skipBlockHeader{};
                skipBlockHeader.countAndOffset.count = 1;
                skipBlockHeader.commands[0].compressed = ipak_consts::IPAK_COMMAND_SKIP;
                skipBlockHeader.commands[0].size = sizeToSkip - sizeof(IPakDataBlockHeader);
                Write(&skipBlockHeader, sizeof(skipBlockHeader));
            }

            AlignToChunk();
            m_chunk_buffer_window_start = m_current_offset;
        }

        void StartNewBlock()
        {
            AlignToBlockHeader();

            // Skip to the next chunk when only the header could fit into the current chunk anyway
            if (static_cast<size_t>(utils::Align(m_current_offset, static_cast<int64_t>(ipak_consts::IPAK_CHUNK_SIZE)) - m_current_offset)
                <= sizeof(IPakDataBlockHeader))
                FlushChunk();

            m_current_block_header_offset = m_current_offset;
            m_current_block = {};
            m_current_block.countAndOffset.offset = static_cast<uint32_t>(m_file_offset);

            // Reserve space to later write actual block header data
            GoTo(m_current_offset + sizeof(IPakDataBlockHeader));
        }

        void WriteChunkData(const void* data, const size_t dataSize)
        {
            auto dataOffset = 0u;
            while (dataOffset < dataSize)
            {
                if (m_current_block.countAndOffset.count >= std::extent_v<decltype(IPakDataBlockHeader::commands)>)
                {
                    FlushBlock();
                    StartNewBlock();
                }

                const auto remainingSize = dataSize - dataOffset;
                const auto remainingChunkBufferWindowSize = std::max((ipak_consts::IPAK_CHUNK_COUNT_PER_READ * ipak_consts::IPAK_CHUNK_SIZE)
                                                                         - static_cast<size_t>(m_current_offset - m_chunk_buffer_window_start),
                                                                     0u);

                if (remainingChunkBufferWindowSize == 0)
                {
                    FlushChunk();
                    StartNewBlock();
                    continue;
                }

                const auto commandSize = std::min(std::min(remainingSize, ipak_consts::IPAK_COMMAND_DEFAULT_SIZE), remainingChunkBufferWindowSize);

                auto writeUncompressed = true;
                if (USE_IPAK_COMPRESSION)
                {
                    auto outLen = static_cast<lzo_uint>(ipak_consts::IPAK_CHUNK_SIZE);
                    const auto result = lzo1x_1_compress(&static_cast<const unsigned char*>(data)[dataOffset],
                                                         commandSize,
                                                         reinterpret_cast<unsigned char*>(m_decompressed_buffer.get()),
                                                         &outLen,
                                                         m_lzo_work_buffer.get());

                    if (result == LZO_E_OK && outLen < commandSize)
                    {
                        writeUncompressed = false;
                        Write(m_decompressed_buffer.get(), outLen);

                        const auto currentCommand = m_current_block.countAndOffset.count;
                        m_current_block.commands[currentCommand].size = static_cast<uint32_t>(outLen);
                        m_current_block.commands[currentCommand].compressed = ipak_consts::IPAK_COMMAND_COMPRESSED;
                        m_current_block.countAndOffset.count = currentCommand + 1u;
                    }
                }

                if (writeUncompressed)
                {
                    Write(&static_cast<const char*>(data)[dataOffset], commandSize);

                    const auto currentCommand = m_current_block.countAndOffset.count;
                    m_current_block.commands[currentCommand].size = commandSize;
                    m_current_block.commands[currentCommand].compressed = ipak_consts::IPAK_COMMAND_UNCOMPRESSED;
                    m_current_block.countAndOffset.count = currentCommand + 1u;
                }

                dataOffset += commandSize;
                m_file_offset += commandSize;
            }
        }

        void StartNewFile()
        {
            FlushBlock();

            m_file_offset = 0u;
            StartNewBlock();
            m_chunk_buffer_window_start = utils::AlignToPrevious(m_current_offset, static_cast<int64_t>(ipak_consts::IPAK_CHUNK_SIZE));
        }

        bool WriteImageData(const std::string& imageName)
        {
            size_t imageSize;
            const auto imageData = ReadImageDataFromSearchPath(imageName, imageSize);
            if (!imageData)
                return false;

            const auto nameHash = T6::Common::R_HashString(imageName.c_str(), 0);
            const auto dataHash = static_cast<unsigned>(crc32(0u, reinterpret_cast<const Bytef*>(imageData.get()), imageSize));

            StartNewFile();
            const auto startOffset = m_current_block_header_offset;

            IPakIndexEntry indexEntry;
            indexEntry.key.nameHash = nameHash;
            indexEntry.key.dataHash = dataHash & 0x1FFFFFFF;
            indexEntry.offset = static_cast<uint32_t>(startOffset - m_data_section_offset);

            WriteChunkData(imageData.get(), imageSize);
            const auto writtenImageSize = static_cast<size_t>(m_current_offset - startOffset);

            indexEntry.size = writtenImageSize;
            m_index_entries.emplace_back(indexEntry);

            return true;
        }

        bool WriteDataSection()
        {
            AlignToChunk();
            m_data_section_offset = m_current_offset;
            m_data_section_size = 0u;

            m_index_entries.reserve(m_images.size());

            const auto result = std::ranges::all_of(m_images,
                                                    [this](const std::string& imageName)
                                                    {
                                                        return WriteImageData(imageName);
                                                    });

            FlushBlock();
            m_data_section_size = static_cast<size_t>(m_current_offset - m_data_section_offset);

            return result;
        }

        static bool CompareIndices(const IPakIndexEntry& entry1, const IPakIndexEntry& entry2)
        {
            return entry1.key.combinedKey < entry2.key.combinedKey;
        }

        void SortIndexSectionEntries()
        {
            std::ranges::sort(m_index_entries, CompareIndices);
        }

        void WriteIndexSection()
        {
            AlignToChunk();
            m_index_section_offset = m_current_offset;

            SortIndexSectionEntries();

            for (const auto& indexEntry : m_index_entries)
                Write(&indexEntry, sizeof(indexEntry));
        }

        void WriteBrandingSection()
        {
            AlignToChunk();
            m_branding_section_offset = m_current_offset;

            Write(BRANDING, std::extent_v<decltype(BRANDING)>);
        }

        void WriteFileEnding()
        {
            AlignToChunk();
            m_total_size = m_current_offset;
        }

        std::ostream& m_stream;
        ISearchPath& m_search_path;
        const std::vector<std::string>& m_images;

        int64_t m_current_offset;
        std::vector<IPakIndexEntry> m_index_entries;
        int64_t m_total_size;
        int64_t m_data_section_offset;
        size_t m_data_section_size;
        int64_t m_index_section_offset;
        int64_t m_branding_section_offset;

        std::unique_ptr<char[]> m_decompressed_buffer;
        std::unique_ptr<char[]> m_lzo_work_buffer;
        size_t m_file_offset;
        int64_t m_chunk_buffer_window_start;
        IPakDataBlockHeader m_current_block;
        int64_t m_current_block_header_offset;
    };
} // namespace

IPakToCreate::IPakToCreate(std::string name)
    : m_name(std::move(name))
{
}

void IPakToCreate::AddImage(std::string imageName)
{
    m_image_names.emplace_back(std::move(imageName));
}

void IPakToCreate::Build(ISearchPath& searchPath, const std::filesystem::path& outPath)
{
    auto filePath = outPath / std::format("{}.ipak", m_name);
    std::ofstream file(filePath, std::ios::out | std::ios::binary);
    if (!file.is_open())
    {
        std::cerr << std::format("Failed to open file for ipak {}\n", m_name);
        return;
    }

    IPakWriter writer(file, searchPath, m_image_names);
    writer.Write();

    std::cout << std::format("Created ipak {} with {} entries\n", m_name, m_image_names.size());
}

IPakToCreate* IPakCreator::GetOrAddIPak(const std::string& ipakName)
{
    const auto existingIPak = m_ipak_lookup.find(ipakName);
    if (existingIPak != m_ipak_lookup.end())
        return existingIPak->second;

    auto newIPak = std::make_unique<IPakToCreate>(ipakName);
    auto* result = newIPak.get();
    m_ipaks.emplace_back(std::move(newIPak));

    return result;
}

void IPakCreator::Finalize(ISearchPath& searchPath, const std::filesystem::path& outPath)
{
    for (const auto& ipakToCreate : m_ipaks)
        ipakToCreate->Build(searchPath, outPath);

    m_ipaks.clear();
    m_ipak_lookup.clear();
}
