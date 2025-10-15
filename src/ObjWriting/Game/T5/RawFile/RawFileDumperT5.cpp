#include "RawFileDumperT5.h"

#include "Utils/Logging/Log.h"
#include "Utils/StringUtils.h"

#include <cassert>
#include <filesystem>
#include <zlib.h>

using namespace T5;

namespace fs = std::filesystem;

namespace
{
    constexpr static size_t GSC_MAX_SIZE = 0xC000000;

    void DumpGsc(AssetDumpingContext& context, const XAssetInfo<RawFile>& asset, std::ostream& stream)
    {
        const auto* rawFile = asset.Asset();

        if (rawFile->len <= 8)
        {
            con::error("Invalid len of gsc file \"{}\"", rawFile->name);
            return;
        }

        const auto outLen = reinterpret_cast<const uint32_t*>(rawFile->buffer)[0];
        const auto inLen = reinterpret_cast<const uint32_t*>(rawFile->buffer)[1];

        assert(inLen == static_cast<unsigned>(rawFile->len) - 8);

        if (inLen > static_cast<unsigned>(rawFile->len - 8) + 1)
        {
            con::error("Invalid compression of gsc file \"{}\": {}", rawFile->name, inLen);
            return;
        }

        if (outLen > GSC_MAX_SIZE)
        {
            con::error("Invalid size of gsc file \"{}\": {}", rawFile->name, outLen);
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
            zs.avail_out = sizeof(buffer);
            ret = inflate(&zs, Z_SYNC_FLUSH);

            if (ret < 0)
            {
                con::error("Inflate failed for dumping gsc file \"{}\"", rawFile->name);
                inflateEnd(&zs);
                return;
            }

            const auto inflateOutSize = sizeof(buffer) - zs.avail_out;

            if (writtenSize + inflateOutSize >= outLen)
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
} // namespace

namespace raw_file
{
    DumperT5::DumperT5(const AssetPool<AssetRawFile::Type>& pool)
        : AbstractAssetDumper(pool)
    {
    }

    void DumperT5::DumpAsset(AssetDumpingContext& context, const XAssetInfo<AssetRawFile::Type>& asset)
    {
        const auto* rawFile = asset.Asset();
        const auto assetFile = context.OpenAssetFile(asset.m_name);

        if (!assetFile)
            return;

        auto& stream = *assetFile;
        const fs::path rawFilePath(rawFile->name);
        auto extension = rawFilePath.extension().string();
        utils::MakeStringLowerCase(extension);

        if (extension == ".gsc" || extension == ".csc")
            DumpGsc(context, asset, stream);
        else
            stream.write(rawFile->buffer, rawFile->len);
    }
} // namespace raw_file
