#include "LoaderRawFileT6.h"

#include "Game/T6/T6.h"
#include "Pool/GlobalAssetPool.h"
#include "Utils/Logging/Log.h"

#include <cstring>
#include <filesystem>
#include <format>
#include <iostream>
#include <zlib.h>
#include <zutil.h>

using namespace T6;
namespace fs = std::filesystem;

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

            const fs::path rawFilePath(assetName);
            const auto extension = rawFilePath.extension().string();

            if (extension == ".atr")
                return LoadAnimtree(file, assetName, context);

            return LoadDefault(file, assetName, context);
        }

    private:
        AssetCreationResult LoadAnimtree(const SearchPathOpenFile& file, const std::string& assetName, AssetCreationContext& context)
        {
            const auto uncompressedBuffer = std::make_unique<char[]>(static_cast<size_t>(file.m_length));
            file.m_stream->read(uncompressedBuffer.get(), file.m_length);
            if (file.m_stream->gcount() != file.m_length)
                return AssetCreationResult::Failure();

            const auto compressionBufferSize = static_cast<size_t>(file.m_length + sizeof(uint32_t) + COMPRESSED_BUFFER_SIZE_PADDING);
            auto* compressedBuffer = m_memory.Alloc<char>(compressionBufferSize);

            z_stream_s zs{};

            zs.zalloc = Z_NULL;
            zs.zfree = Z_NULL;
            zs.opaque = Z_NULL;
            zs.avail_in = static_cast<uInt>(file.m_length);
            zs.avail_out = static_cast<unsigned>(compressionBufferSize);
            zs.next_in = reinterpret_cast<const Bytef*>(uncompressedBuffer.get());
            zs.next_out = reinterpret_cast<Bytef*>(&compressedBuffer[sizeof(uint32_t)]);

            int ret = deflateInit2(&zs, Z_DEFAULT_COMPRESSION, Z_DEFLATED, -DEF_WBITS, DEF_MEM_LEVEL, Z_DEFAULT_STRATEGY);

            if (ret != Z_OK)
            {
                throw std::runtime_error("Initializing deflate failed");
            }

            ret = deflate(&zs, Z_FINISH);

            if (ret != Z_STREAM_END)
            {
                con::error("Deflate failed for loading animtree file \"{}\"", assetName);
                deflateEnd(&zs);
                return AssetCreationResult::Failure();
            }

            const auto compressedSize = compressionBufferSize + sizeof(uint32_t) - zs.avail_out;

            reinterpret_cast<uint32_t*>(compressedBuffer)[0] = static_cast<uint32_t>(file.m_length); // outLen

            auto* rawFile = m_memory.Alloc<RawFile>();
            rawFile->name = m_memory.Dup(assetName.c_str());
            rawFile->len = static_cast<int>(compressedSize);
            rawFile->buffer = static_cast<const char*>(compressedBuffer);

            deflateEnd(&zs);

            return AssetCreationResult::Success(context.AddAsset<AssetRawFile>(assetName, rawFile));
        }

        AssetCreationResult LoadDefault(const SearchPathOpenFile& file, const std::string& assetName, AssetCreationContext& context) const
        {
            auto* rawFile = m_memory.Alloc<RawFile>();
            rawFile->name = m_memory.Dup(assetName.c_str());
            rawFile->len = static_cast<int>(file.m_length);

            auto* fileBuffer = m_memory.Alloc<char>(static_cast<size_t>(file.m_length + 1));
            file.m_stream->read(fileBuffer, file.m_length);
            if (file.m_stream->gcount() != file.m_length)
                return AssetCreationResult::Failure();
            fileBuffer[rawFile->len] = '\0';

            rawFile->buffer = static_cast<char16*>(fileBuffer);

            return AssetCreationResult::Success(context.AddAsset<AssetRawFile>(assetName, rawFile));
        }

        MemoryManager& m_memory;
        ISearchPath& m_search_path;
    };
} // namespace

namespace raw_file
{
    std::unique_ptr<AssetCreator<AssetRawFile>> CreateLoaderT6(MemoryManager& memory, ISearchPath& searchPath)
    {
        return std::make_unique<RawFileLoader>(memory, searchPath);
    }
} // namespace raw_file
