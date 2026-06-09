#include "D3DBspReaderIW3.h"

#include <algorithm>
#include <cstring>
#include <format>
#include <limits>
#include <utility>

using namespace IW3::d3dbsp;

namespace
{
    constexpr auto BSP_HEADER_SIZE = 12uz;
    constexpr auto BSP_LUMP_ENTRY_SIZE = 8uz;

    // IW3 v22 uses a typed chunk table after the IBSP/version header. The stock
    // linker/Radiant path rejects chunk counts above this limit.
    constexpr auto MAX_CHUNK_COUNT = 100u;

    [[nodiscard]] size_t Align4(const size_t value)
    {
        return (value + 3uz) & ~3uz;
    }

    [[nodiscard]] bool ReadExact(std::istream& stream, std::vector<std::byte>& out, const int64_t length)
    {
        if (length < 0 || static_cast<uint64_t>(length) > static_cast<uint64_t>(std::numeric_limits<size_t>::max()))
            return false;

        out.resize(static_cast<size_t>(length));
        if (out.empty())
            return true;

        stream.read(reinterpret_cast<char*>(out.data()), length);
        return stream.gcount() == length;
    }

    template<typename T> [[nodiscard]] bool ReadValue(const std::vector<std::byte>& data, size_t& offset, T& value)
    {
        if (offset > data.size() || sizeof(T) > data.size() - offset)
            return false;

        std::memcpy(&value, data.data() + offset, sizeof(T));
        offset += sizeof(T);
        return true;
    }

    [[nodiscard]] LoadResult InvalidResult(std::string message)
    {
        LoadResult result;
        result.status = LoadStatus::Invalid;
        result.message = std::move(message);
        return result;
    }

    struct LumpEntry
    {
        uint32_t id;
        uint32_t size;
    };

    [[nodiscard]] LoadResult ParseBsp(std::vector<std::byte>&& data, std::string fileName)
    {
        if (data.size() < BSP_HEADER_SIZE)
            return InvalidResult(std::format("{} is too small to be an IW3 d3dbsp", fileName));

        if (!std::equal(BSP_MAGIC.begin(), BSP_MAGIC.end(), reinterpret_cast<const char*>(data.data())))
            return InvalidResult(std::format("{} does not start with IBSP", fileName));

        size_t offset = BSP_MAGIC.size();

        uint32_t version = 0;
        if (!ReadValue(data, offset, version))
            return InvalidResult(std::format("{} has a truncated BSP version", fileName));

        if (version != BSP_VERSION)
            return InvalidResult(std::format("{} is BSP version {}, expected {}", fileName, version, BSP_VERSION));

        uint32_t chunkCount = 0;
        if (!ReadValue(data, offset, chunkCount))
            return InvalidResult(std::format("{} has a truncated BSP chunk count", fileName));

        if (chunkCount > MAX_CHUNK_COUNT)
            return InvalidResult(std::format("{} has an unreasonable BSP chunk count {}", fileName, chunkCount));

        const auto tableSize = static_cast<size_t>(chunkCount) * BSP_LUMP_ENTRY_SIZE;
        if (offset > data.size() || tableSize > data.size() - offset)
            return InvalidResult(std::format("{} has a truncated BSP chunk table", fileName));

        std::vector<LumpEntry> entries;
        entries.reserve(chunkCount);

        for (auto i = 0u; i < chunkCount; i++)
        {
            LumpEntry entry{};
            if (!ReadValue(data, offset, entry.id) || !ReadValue(data, offset, entry.size))
                return InvalidResult(std::format("{} has a truncated BSP chunk table entry", fileName));

            entries.emplace_back(entry);
        }

        auto parsed = std::make_unique<File>(std::move(fileName));
        parsed->m_lumps.reserve(entries.size());

        for (const auto& entry : entries)
        {
            const auto lumpSize = static_cast<size_t>(entry.size);
            if (offset > data.size() || lumpSize > data.size() - offset)
                return InvalidResult(std::format("{} lump {} extends past end of file", parsed->m_file_name, entry.id));

            Lump lump;
            lump.id = static_cast<LumpType>(entry.id);
            lump.data.resize(lumpSize);
            if (lumpSize > 0)
                std::memcpy(lump.data.data(), data.data() + offset, lumpSize);

            parsed->m_lumps.emplace_back(std::move(lump));

            const auto alignedSize = Align4(lumpSize);
            if (alignedSize > data.size() - offset)
                return InvalidResult(std::format("{} lump {} padding extends past end of file", parsed->m_file_name, entry.id));

            offset += alignedSize;
        }

        LoadResult result;
        result.status = LoadStatus::Loaded;
        result.file = std::move(parsed);
        return result;
    }
} // namespace

File::File(std::string fileName)
    : m_file_name(std::move(fileName))
{
}

const Lump* File::GetLump(const LumpType id) const
{
    for (const auto& lump : m_lumps)
    {
        if (lump.id == id)
            return &lump;
    }

    return nullptr;
}

LoadResult IW3::d3dbsp::LoadFromSearchPath(const std::string& assetName, ISearchPath& searchPath)
{
    auto openFile = searchPath.Open(assetName);
    if (!openFile.IsOpen())
    {
        LoadResult result;
        result.status = LoadStatus::NotFound;
        return result;
    }

    std::vector<std::byte> data;
    if (!ReadExact(*openFile.m_stream, data, openFile.m_length))
        return InvalidResult(std::format("Could not read d3dbsp file {}", assetName));

    return ParseBsp(std::move(data), assetName);
}
