#include "AssetDumperRawFile.h"

#include <filesystem>
#include <zlib.h>
#include <zutil.h>

using namespace T6;

namespace fs = std::filesystem;

bool AssetDumperRawFile::ShouldDump(XAssetInfo<RawFile>* asset)
{
    return true;
}

void AssetDumperRawFile::DumpAnimtree(AssetDumpingContext& context, XAssetInfo<RawFile>* asset, std::ostream& stream)
{
    const auto* rawFile = asset->Asset();

    if (rawFile->len <= 4)
    {
        std::cerr << "Invalid len of animtree file \"" << rawFile->name << "\"" << std::endl;
        return;
    }

    const auto outLen = reinterpret_cast<const uint32_t*>(rawFile->buffer)[0];
    const auto inLen = rawFile->len;

    if (outLen > ANIMTREE_MAX_SIZE)
    {
        std::cerr << "Invalid size of animtree file \"" << rawFile->name << "\": " << outLen << std::endl;
        return;
    }

    z_stream_s zs{};

    zs.zalloc = Z_NULL;
    zs.zfree = Z_NULL;
    zs.opaque = Z_NULL;
    zs.avail_in = 0;
    zs.next_in = Z_NULL;

    int ret = inflateInit2(&zs, -DEF_WBITS);

    if (ret != Z_OK)
    {
        throw std::runtime_error("Initializing inflate failed");
    }

    zs.next_in = reinterpret_cast<const Bytef*>(&rawFile->buffer[4]);
    zs.avail_in = inLen - sizeof(uint32_t);

    Bytef buffer[0x1000];

    while (zs.avail_in > 0)
    {
        zs.next_out = buffer;
        zs.avail_out = sizeof buffer;
        ret = inflate(&zs, Z_SYNC_FLUSH);

        if (ret < 0)
        {
            std::cerr << "Inflate failed for dumping animtree file \"" << rawFile->name << "\"" << std::endl;
            inflateEnd(&zs);
            return;
        }

        const auto inflateOutSize = sizeof buffer - zs.avail_out;

        stream.write(reinterpret_cast<char*>(buffer), inflateOutSize);
    }

    inflateEnd(&zs);
}

void AssetDumperRawFile::DumpAsset(AssetDumpingContext& context, XAssetInfo<RawFile>* asset)
{
    const auto* rawFile = asset->Asset();
    const auto assetFile = context.OpenAssetFile(asset->m_name);

    if (!assetFile)
        return;

    auto& stream = *assetFile;
    const fs::path rawFilePath(rawFile->name);
    const auto extension = rawFilePath.extension().string();

    if (extension == ".atr")
    {
        DumpAnimtree(context, asset, stream);
    }
    else
    {
        stream.write(rawFile->buffer, rawFile->len);
    }
}
