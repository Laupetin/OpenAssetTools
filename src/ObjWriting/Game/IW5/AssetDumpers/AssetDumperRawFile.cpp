#include "AssetDumperRawFile.h"

#include <format>
#include <stdexcept>
#include <zlib.h>

using namespace IW5;

bool AssetDumperRawFile::ShouldDump(XAssetInfo<RawFile>* asset)
{
    return true;
}

void AssetDumperRawFile::DumpAsset(AssetDumpingContext& context, XAssetInfo<RawFile>* asset)
{
    const auto* rawFile = asset->Asset();
    const auto assetFile = context.OpenAssetFile(asset->m_name);

    if (!assetFile)
        return;

    auto& stream = *assetFile;
    if (rawFile->compressedLen <= 0)
        return;

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

    zs.next_in = reinterpret_cast<const Bytef*>(rawFile->buffer);
    zs.avail_in = rawFile->compressedLen;

    Bytef buffer[0x1000];

    while (zs.avail_in > 0)
    {
        zs.next_out = buffer;
        zs.avail_out = sizeof(buffer);
        ret = inflate(&zs, Z_SYNC_FLUSH);

        if (ret < 0)
        {
            std::cerr << std::format("Inflate failed when attempting to dump rawfile '{}'\n", rawFile->name);
            inflateEnd(&zs);
            return;
        }

        stream.write(reinterpret_cast<char*>(buffer), sizeof(buffer) - zs.avail_out);
    }

    inflateEnd(&zs);
}
