#include "ImageLoaderCommon.h"

#include "Image/ImageCommon.h"
#include "Image/IwiLoader.h"
#include "Image/Texture.h"
#include "Utils/Logging/Log.h"

#include <sstream>
#include <zlib.h>

using namespace image;

namespace
{
    CommonImageLoaderResult Success(size_t iwiSize, CommonIwiMetaData meta, std::unique_ptr<Texture> texture, CommonImageLoaderHash hash)
    {
        return CommonImageLoaderResult{
            .m_failure = false,
            .m_iwi_size = iwiSize,
            .m_meta = meta,
            .m_texture = std::move(texture),
            .m_hash = hash,
        };
    }

    CommonImageLoaderResult NoAction()
    {
        return CommonImageLoaderResult{
            .m_failure = false,
        };
    }

    CommonImageLoaderResult Failure()
    {
        return CommonImageLoaderResult{
            .m_failure = true,
        };
    }
} // namespace

namespace image
{
    std::optional<AssetCreationResult> CommonImageLoaderResult::GetResultIfCancelled() const
    {
        if (m_failure)
            return AssetCreationResult::Failure();

        if (!m_texture)
            return AssetCreationResult::NoAction();

        return std::nullopt;
    }

    CommonImageLoaderResult
        LoadImageCommon(const std::string& imageName, ISearchPath& searchPath, IwiVersion expectedIwiVersion, CommonImageLoaderHashType hashType)
    {
        const auto fileName = image::GetFileNameForAsset(imageName, ".iwi");
        const auto file = searchPath.Open(fileName);
        if (!file.IsOpen())
            return NoAction();

        const auto fileSize = static_cast<size_t>(file.m_length);

        std::optional<IwiLoaderResult> loaderResult;
        CommonImageLoaderHash hash{};
        if (hashType == CommonImageLoaderHashType::NONE)
        {
            loaderResult = image::LoadIwi(*file.m_stream);
        }
        else
        {
            const auto fileData = std::make_unique<char[]>(fileSize);
            file.m_stream->read(fileData.get(), static_cast<std::streamsize>(fileSize));

            hash.crc32 = static_cast<unsigned>(crc32(0u, reinterpret_cast<const Bytef*>(fileData.get()), static_cast<unsigned>(fileSize)));

            std::istringstream inMemory(std::string(fileData.get(), fileSize));
            loaderResult = image::LoadIwi(inMemory);
        }

        if (!loaderResult)
        {
            con::error("Failed to load texture from: {}", fileName);
            return Failure();
        }

        return Success(fileSize, loaderResult->m_meta, std::move(loaderResult->m_texture), hash);
    }
} // namespace image
