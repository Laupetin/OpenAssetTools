#include "IPakWriter.h"

#include <algorithm>
#include <iostream>
#include <sstream>
#include <zlib.h>

#include "Game/T6/CommonT6.h"
#include "Game/T6/GameT6.h"
#include "ObjContainer/IPak/IPakTypes.h"
#include "Utils/Alignment.h"

class IPakWriterImpl final : public IPakWriter
{
    static constexpr char BRANDING[] = "Created with OAT - OpenAssetTools";
    static constexpr auto SECTION_COUNT = 3; // Index + Data + Branding

    inline static const std::string PAD_DATA = std::string(256, '\xA7');

public:
    explicit IPakWriterImpl(std::ostream& stream, ISearchPath* assetSearchPath)
        : m_stream(stream),
          m_asset_search_path(assetSearchPath),
          m_current_offset(0),
          m_total_size(0),
          m_data_section_offset(0),
          m_data_section_size(0u),
          m_index_section_offset(0),
          m_branding_section_offset(0)
    {
    }

    void AddImage(std::string imageName) override
    {
        m_images.emplace_back(std::move(imageName));
    }

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

    void AlignToChunkWrite()
    {
        Pad(static_cast<size_t>(utils::Align(m_current_offset, 0x8000i64) - m_current_offset));
    }

    void WriteHeaderData()
    {
        GoTo(0);

        const IPakHeader header{
            ipak_consts::IPAK_MAGIC,
            ipak_consts::IPAK_VERSION,
            static_cast<uint32_t>(m_total_size),
            SECTION_COUNT
        };

        const IPakSection dataSection{
            ipak_consts::IPAK_DATA_SECTION,
            static_cast<uint32_t>(m_data_section_offset),
            static_cast<uint32_t>(m_data_section_size),
            static_cast<uint32_t>(m_index_entries.size())
        };

        const IPakSection indexSection{
            ipak_consts::IPAK_INDEX_SECTION,
            static_cast<uint32_t>(m_index_section_offset),
            static_cast<uint32_t>(sizeof(IPakIndexEntry) * m_index_entries.size()),
            static_cast<uint32_t>(m_index_entries.size())
        };

        const IPakSection brandingSection{
            ipak_consts::IPAK_BRANDING_SECTION,
            static_cast<uint32_t>(m_branding_section_offset),
            std::extent_v<decltype(BRANDING)>,
            1
        };

        Write(&header, sizeof(header));
        Write(&dataSection, sizeof(dataSection));
        Write(&indexSection, sizeof(indexSection));
        Write(&brandingSection, sizeof(brandingSection));
    }

    static std::string ImageFileName(const std::string& imageName)
    {
        std::ostringstream ss;
        ss << "images/" << imageName << ".iwi";

        return ss.str();
    }

    std::unique_ptr<char[]> ReadImageDataFromSearchPath(const std::string& imageName, size_t& imageSize) const
    {
        const auto fileName = ImageFileName(imageName);

        const auto openFile = m_asset_search_path->Open(fileName);
        if (!openFile.IsOpen())
        {
            std::cerr << "Could not open image for writing to IPak \"" << fileName << "\"\n";
            return nullptr;
        }

        imageSize = static_cast<size_t>(openFile.m_length);
        auto imageData = std::make_unique<char[]>(imageSize);
        openFile.m_stream->read(imageData.get(), imageSize);

        return imageData;
    }

    bool WriteImageData(const std::string& imageName)
    {
        size_t imageSize;
        const auto imageData = ReadImageDataFromSearchPath(imageName, imageSize);
        if (!imageData)
            return false;

        const auto nameHash = T6::Common::R_HashString(imageName.c_str(), 0);
        const auto dataHash = static_cast<unsigned>(crc32(0u, reinterpret_cast<const Bytef*>(imageData.get()), imageSize));

        IPakIndexEntry indexEntry{};
        indexEntry.key.nameHash = nameHash;
        indexEntry.key.dataHash = dataHash & 0x1FFFFFFF;
        indexEntry.offset = static_cast<uint32_t>(m_current_offset - m_data_section_offset);

        size_t writtenImageSize = 0u;

        // TODO: Write image data

        indexEntry.size = writtenImageSize;
        m_index_entries.emplace_back(indexEntry);

        m_data_section_size += writtenImageSize;
        return true;
    }

    bool WriteDataSection()
    {
        AlignToChunkWrite();
        m_data_section_offset = m_current_offset;
        m_data_section_size = 0u;

        m_index_entries.reserve(m_images.size());

        return std::all_of(m_images.begin(), m_images.end(), [this](const std::string& imageName)
        {
            return WriteImageData(imageName);
        });
    }

    static bool CompareIndices(const IPakIndexEntry& entry1, const IPakIndexEntry& entry2)
    {
        return entry1.key.combinedKey < entry2.key.combinedKey;
    }

    void SortIndexSectionEntries()
    {
        std::sort(m_index_entries.begin(), m_index_entries.end(), CompareIndices);
    }

    void WriteIndexSection()
    {
        AlignToChunkWrite();
        m_index_section_offset = m_current_offset;

        SortIndexSectionEntries();

        for (const auto& indexEntry : m_index_entries)
            Write(&indexEntry, sizeof(indexEntry));
    }

    void WriteBrandingSection()
    {
        AlignToChunkWrite();
        m_branding_section_offset = m_current_offset;

        Write(BRANDING, std::extent_v<decltype(BRANDING)>);
    }

    void WriteFileEnding()
    {
        AlignToChunkWrite();
        m_total_size = m_current_offset;
    }

    bool Write() override
    {
        // We will write the header and sections later since they need complementary data
        GoTo(sizeof(IPakHeader) + sizeof(IPakSection) * SECTION_COUNT);
        AlignToChunkWrite();

        if (!WriteDataSection())
            return false;

        WriteIndexSection();
        WriteBrandingSection();
        WriteFileEnding();

        WriteHeaderData();

        return true;
    }

private:
    std::ostream& m_stream;
    ISearchPath* m_asset_search_path;
    std::vector<std::string> m_images;

    int64_t m_current_offset;
    std::vector<IPakIndexEntry> m_index_entries;
    int64_t m_total_size;
    int64_t m_data_section_offset;
    size_t m_data_section_size;
    int64_t m_index_section_offset;
    int64_t m_branding_section_offset;
};

std::unique_ptr<IPakWriter> IPakWriter::Create(std::ostream& stream, ISearchPath* assetSearchPath)
{
    return std::make_unique<IPakWriterImpl>(stream, assetSearchPath);
}
