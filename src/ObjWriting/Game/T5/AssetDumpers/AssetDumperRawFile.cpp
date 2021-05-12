#include "AssetDumperRawFile.h"

#include <cassert>
#include <filesystem>
#include <zlib.h>

using namespace T5;

namespace fs = std::filesystem;

void AssetDumperRawFile::DumpGsc(AssetDumpingContext& context, XAssetInfo<RawFile>* asset, std::ostream& stream)
{
    const auto* rawFile = asset->Asset();

    if (rawFile->len <= 8)
    {
        std::cout << "Invalid len of gsc file \"" << rawFile->name << "\"" << std::endl;
        return;
    }

    const auto outLen = reinterpret_cast<const uint32_t*>(rawFile->buffer)[0];
    const auto inLen = reinterpret_cast<const uint32_t*>(rawFile->buffer)[1];

    assert(inLen == static_cast<unsigned>(rawFile->len) - 8);

    if(inLen > static_cast<unsigned>(rawFile->len - 8) + 1)
    {
        std::cout << "Invalid compression of gsc file \"" << rawFile->name << "\": " << inLen << std::endl;
        return;
    }

    if(outLen > GSC_MAX_SIZE)
    {
        std::cout << "Invalid size of gsc file \"" << rawFile->name << "\": " << outLen << std::endl;
        return;
    }

    z_stream_s zs{};

    zs.zalloc = Z_NULL;
    zs.zfree = Z_NULL;
    zs.opaque = Z_NULL;
    zs.avail_in = 0;
    zs.next_in = Z_NULL;

    int ret = inflateInit(&zs);

    if (ret != Z_OK)
    {
        throw std::runtime_error("Initializing inflate failed");
    }

    zs.next_in = reinterpret_cast<const Bytef*>(&rawFile->buffer[8]);
    zs.avail_in = inLen;

    Bytef buffer[0x1000];

    size_t writtenSize = 0;
    while (zs.avail_in > 0)
    {
        zs.next_out = buffer;
        zs.avail_out = sizeof buffer;
        ret = inflate(&zs, Z_SYNC_FLUSH);

        if (ret < 0)
        {
            std::cout << "Inflate failed for dumping gsc file \"" << rawFile->name << "\"" << std::endl;
            inflateEnd(&zs);
            return;
        }

        const auto inflateOutSize = sizeof buffer - zs.avail_out;

        if(writtenSize + inflateOutSize >= outLen)
        {
            // Last byte is a \0 byte. Skip it.
            stream.write(reinterpret_cast<char*>(buffer), inflateOutSize - 1);
        }
        else
        {
            stream.write(reinterpret_cast<char*>(buffer), inflateOutSize);
        }
        writtenSize += inflateOutSize;
    }

    inflateEnd(&zs);
}

bool AssetDumperRawFile::ShouldDump(XAssetInfo<RawFile>* asset)
{
    return true;
}

bool AssetDumperRawFile::CanDumpAsRaw()
{
    return true;
}

std::string AssetDumperRawFile::GetFileNameForAsset(Zone* zone, XAssetInfo<RawFile>* asset)
{
    return asset->m_name;
}

void AssetDumperRawFile::DumpRaw(AssetDumpingContext& context, XAssetInfo<RawFile>* asset, std::ostream& stream)
{
    const auto* rawFile = asset->Asset();

    const fs::path rawFilePath(rawFile->name);
    const auto extension = rawFilePath.extension().string();

    if(extension == ".gsc" || extension == ".csc")
    {
        DumpGsc(context, asset, stream);
    }
    else
    {
        stream.write(rawFile->buffer, rawFile->len);
    }
}