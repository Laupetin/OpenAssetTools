#include "AssetDumperRawFile.h"

#include <zlib.h>
#include <stdexcept>

using namespace IW5;

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
    std::string cleanAssetName = asset->m_name;
    for (auto& c : cleanAssetName)
    {
        switch (c)
        {
        case '*':
            c = '_';
            break;

        default:
            break;
        }
    }

    return cleanAssetName;
}

void AssetDumperRawFile::DumpRaw(AssetDumpingContext& context, XAssetInfo<RawFile>* asset, std::ostream& stream)
{
    const auto* rawFile = asset->Asset();
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
        zs.avail_out = sizeof buffer;
        ret = inflate(&zs, Z_SYNC_FLUSH);

        if (ret < 0)
        {
            printf("Inflate failed for dumping rawfile '%s'\n", rawFile->name);
            inflateEnd(&zs);
            return;
        }

        stream.write(reinterpret_cast<char*>(buffer), sizeof buffer - zs.avail_out);
    }

    inflateEnd(&zs);
}
