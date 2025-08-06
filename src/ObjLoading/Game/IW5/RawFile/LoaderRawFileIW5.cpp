#include "LoaderRawFileIW5.h"

#include "Game/IW5/IW5.h"
#include "Pool/GlobalAssetPool.h"

#include <cstring>
#include <filesystem>
#include <format>
#include <iostream>
#include <zlib.h>

using namespace IW5;

namespace
{
    constexpr size_t COMPRESSED_BUFFER_SIZE_PADDING = 64;

    class RawFileLoader final : public AssetCreator<AssetRawFile>
    {
    public:
        RawFileLoader(MemoryManager& memory, ISearchPath& searchPath)
            : m_memory(memory),
              m_search_path(searchPath)
        {
        }

        AssetCreationResult CreateAsset(const std::string& assetName, AssetCreationContext& context) override
        {
            const auto file = m_search_path.Open(assetName);
            if (!file.IsOpen())
                return AssetCreationResult::NoAction();

            const auto uncompressedBuffer = std::make_unique<char[]>(static_cast<size_t>(file.m_length));
            file.m_stream->read(uncompressedBuffer.get(), file.m_length);
            if (file.m_stream->gcount() != file.m_length)
                return AssetCreationResult::Failure();

            const auto compressionBufferSize = static_cast<size_t>(file.m_length + COMPRESSED_BUFFER_SIZE_PADDING);
            auto* compressedBuffer = m_memory.Alloc<char>(compressionBufferSize);

            z_stream_s zs{};

            zs.zalloc = Z_NULL;
            zs.zfree = Z_NULL;
            zs.opaque = Z_NULL;
            zs.avail_in = static_cast<uInt>(file.m_length);
            zs.avail_out = static_cast<unsigned>(compressionBufferSize);
            zs.next_in = reinterpret_cast<const Bytef*>(uncompressedBuffer.get());
            zs.next_out = reinterpret_cast<Bytef*>(compressedBuffer);

            int ret = deflateInit(&zs, Z_DEFAULT_COMPRESSION);

            if (ret != Z_OK)
                throw std::runtime_error("Initializing deflate failed");

            ret = deflate(&zs, Z_FINISH);

            if (ret != Z_STREAM_END)
            {
                std::cerr << std::format("Deflate failed for loading rawfile \"{}\"\n", assetName);
                deflateEnd(&zs);
                return AssetCreationResult::Failure();
            }

            const auto compressedSize = compressionBufferSize - zs.avail_out;

            auto* rawFile = m_memory.Alloc<RawFile>();
            rawFile->name = m_memory.Dup(assetName.c_str());
            rawFile->compressedLen = static_cast<int>(compressedSize);
            rawFile->len = static_cast<int>(file.m_length);
            rawFile->buffer = static_cast<const char*>(compressedBuffer);

            deflateEnd(&zs);

            return AssetCreationResult::Success(context.AddAsset<AssetRawFile>(assetName, rawFile));
        }

    private:
        MemoryManager& m_memory;
        ISearchPath& m_search_path;
    };
} // namespace

namespace raw_file
{
    std::unique_ptr<AssetCreator<AssetRawFile>> CreateLoaderIW5(MemoryManager& memory, ISearchPath& searchPath)
    {
        return std::make_unique<RawFileLoader>(memory, searchPath);
    }
} // namespace raw_file
