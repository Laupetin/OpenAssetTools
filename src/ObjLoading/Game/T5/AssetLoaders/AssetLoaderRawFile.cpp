#include "AssetLoaderRawFile.h"

#include "Game/T5/T5.h"
#include "Pool/GlobalAssetPool.h"

#include <cstring>
#include <filesystem>
#include <iostream>
#include <zlib.h>

using namespace T5;

namespace fs = std::filesystem;

void* AssetLoaderRawFile::CreateEmptyAsset(const std::string& assetName, MemoryManager* memory)
{
    auto* rawFile = memory->Create<RawFile>();
    memset(rawFile, 0, sizeof(RawFile));
    rawFile->name = memory->Dup(assetName.c_str());
    return rawFile;
}

bool AssetLoaderRawFile::CanLoadFromRaw() const
{
    return true;
}

bool AssetLoaderRawFile::LoadGsc(
    const SearchPathOpenFile& file, const std::string& assetName, ISearchPath* searchPath, MemoryManager* memory, IAssetLoadingManager* manager)
{
    const auto uncompressedBuffer = std::make_unique<char[]>(static_cast<size_t>(file.m_length + 1));
    file.m_stream->read(uncompressedBuffer.get(), file.m_length);
    if (file.m_stream->gcount() != file.m_length)
        return false;
    uncompressedBuffer[static_cast<size_t>(file.m_length)] = '\0';

    const auto compressionBufferSize = static_cast<size_t>(file.m_length + 1 + sizeof(uint32_t) + sizeof(uint32_t) + COMPRESSED_BUFFER_SIZE_PADDING);
    auto* compressedBuffer = static_cast<char*>(memory->Alloc(compressionBufferSize));

    z_stream_s zs{};

    zs.zalloc = Z_NULL;
    zs.zfree = Z_NULL;
    zs.opaque = Z_NULL;
    zs.avail_in = static_cast<uInt>(file.m_length + 1);
    zs.avail_out = compressionBufferSize;
    zs.next_in = reinterpret_cast<const Bytef*>(uncompressedBuffer.get());
    zs.next_out = reinterpret_cast<Bytef*>(&compressedBuffer[sizeof(uint32_t) + sizeof(uint32_t)]);

    int ret = deflateInit(&zs, Z_DEFAULT_COMPRESSION);

    if (ret != Z_OK)
    {
        throw std::runtime_error("Initializing deflate failed");
    }

    ret = deflate(&zs, Z_FINISH);

    if (ret != Z_STREAM_END)
    {
        std::cout << "Deflate failed for loading gsc file \"" << assetName << "\"" << std::endl;
        deflateEnd(&zs);
        return false;
    }

    const auto compressedSize = compressionBufferSize - zs.avail_out;

    reinterpret_cast<uint32_t*>(compressedBuffer)[0] = static_cast<uint32_t>(file.m_length + 1); // outLen
    reinterpret_cast<uint32_t*>(compressedBuffer)[1] = compressedSize;                           // inLen

    auto* rawFile = memory->Create<RawFile>();
    rawFile->name = memory->Dup(assetName.c_str());
    rawFile->len = static_cast<int>(compressedSize + sizeof(uint32_t) + sizeof(uint32_t));
    rawFile->buffer = static_cast<const char*>(compressedBuffer);

    manager->AddAsset(ASSET_TYPE_RAWFILE, assetName, rawFile);

    return true;
}

bool AssetLoaderRawFile::LoadDefault(
    const SearchPathOpenFile& file, const std::string& assetName, ISearchPath* searchPath, MemoryManager* memory, IAssetLoadingManager* manager)
{
    auto* rawFile = memory->Create<RawFile>();
    rawFile->name = memory->Dup(assetName.c_str());
    rawFile->len = static_cast<int>(file.m_length);

    auto* fileBuffer = static_cast<char*>(memory->Alloc(static_cast<size_t>(file.m_length + 1)));
    file.m_stream->read(fileBuffer, file.m_length);
    if (file.m_stream->gcount() != file.m_length)
        return false;
    fileBuffer[rawFile->len] = '\0';

    rawFile->buffer = fileBuffer;
    manager->AddAsset(ASSET_TYPE_RAWFILE, assetName, rawFile);

    return true;
}

bool AssetLoaderRawFile::LoadFromRaw(
    const std::string& assetName, ISearchPath* searchPath, MemoryManager* memory, IAssetLoadingManager* manager, Zone* zone) const
{
    const auto file = searchPath->Open(assetName);
    if (!file.IsOpen())
        return false;

    const fs::path rawFilePath(assetName);
    const auto extension = rawFilePath.extension().string();

    if (extension == ".gsc" || extension == ".csc")
        return LoadGsc(file, assetName, searchPath, memory, manager);

    return LoadDefault(file, assetName, searchPath, memory, manager);
}
