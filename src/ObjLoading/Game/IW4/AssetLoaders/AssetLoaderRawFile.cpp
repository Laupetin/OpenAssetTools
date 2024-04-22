#include "AssetLoaderRawFile.h"

#include "Game/IW4/IW4.h"
#include "Pool/GlobalAssetPool.h"

#include <cstring>
#include <filesystem>
#include <iostream>
#include <zlib.h>

using namespace IW4;

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

bool AssetLoaderRawFile::LoadFromRaw(
    const std::string& assetName, ISearchPath* searchPath, MemoryManager* memory, IAssetLoadingManager* manager, Zone* zone) const
{
    const auto file = searchPath->Open(assetName);
    if (!file.IsOpen())
        return false;

    const auto uncompressedBuffer = std::make_unique<char[]>(static_cast<size_t>(file.m_length));
    file.m_stream->read(uncompressedBuffer.get(), file.m_length);
    if (file.m_stream->gcount() != file.m_length)
        return false;

    const auto compressionBufferSize = static_cast<size_t>(file.m_length + COMPRESSED_BUFFER_SIZE_PADDING);
    auto* compressedBuffer = memory->Alloc<char>(compressionBufferSize);

    z_stream_s zs{};

    zs.zalloc = Z_NULL;
    zs.zfree = Z_NULL;
    zs.opaque = Z_NULL;
    zs.avail_in = static_cast<uInt>(file.m_length);
    zs.avail_out = compressionBufferSize;
    zs.next_in = reinterpret_cast<const Bytef*>(uncompressedBuffer.get());
    zs.next_out = reinterpret_cast<Bytef*>(compressedBuffer);

    int ret = deflateInit(&zs, Z_DEFAULT_COMPRESSION);

    if (ret != Z_OK)
    {
        throw std::runtime_error("Initializing deflate failed");
    }

    ret = deflate(&zs, Z_FINISH);

    if (ret != Z_STREAM_END)
    {
        std::cerr << "Deflate failed for loading rawfile \"" << assetName << "\"\n";
        deflateEnd(&zs);
        return false;
    }

    const auto compressedSize = compressionBufferSize - zs.avail_out;

    auto* rawFile = memory->Create<RawFile>();
    rawFile->name = memory->Dup(assetName.c_str());
    rawFile->compressedLen = static_cast<int>(compressedSize);
    rawFile->len = static_cast<int>(file.m_length);
    rawFile->data.compressedBuffer = static_cast<const char*>(compressedBuffer);

    deflateEnd(&zs);

    manager->AddAsset<AssetRawFile>(assetName, rawFile);

    return true;
}
