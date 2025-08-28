#include "LoaderImageT6.h"

#include "Game/T6/CommonT6.h"
#include "Game/T6/T6.h"
#include "Image/IwiLoader.h"
#include "Image/IwiTypes.h"

#include <cstring>
#include <format>
#include <iostream>
#include <sstream>
#include <zlib.h>

using namespace T6;

namespace
{
    class ImageLoader final : public AssetCreator<AssetImage>
    {
    public:
        ImageLoader(MemoryManager& memory, ISearchPath& searchPath)
            : m_memory(memory),
              m_search_path(searchPath)
        {
        }

        AssetCreationResult CreateAsset(const std::string& assetName, AssetCreationContext& context) override
        {
            auto fileName = std::format("images/{}.iwi", assetName);

            for (size_t i = 0; i < fileName.size(); i++)
                if (fileName[i] == '*')
                    fileName[i] = '_';

            const auto file = m_search_path.Open(fileName);
            if (!file.IsOpen())
                return AssetCreationResult::NoAction();

            const auto fileSize = static_cast<size_t>(file.m_length);
            const auto fileData = std::make_unique<char[]>(fileSize);
            file.m_stream->read(fileData.get(), static_cast<std::streamsize>(fileSize));
            const auto dataHash = static_cast<unsigned>(crc32(0u, reinterpret_cast<const Bytef*>(fileData.get()), static_cast<unsigned>(fileSize)));

            std::istringstream ss(std::string(fileData.get(), fileSize));
            const auto texture = iwi::LoadIwi(ss);
            if (!texture)
            {
                std::cerr << std::format("Failed to load texture from: {}\n", fileName);
                return AssetCreationResult::Failure();
            }

            auto* image = m_memory.Alloc<GfxImage>();
            image->name = m_memory.Dup(assetName.c_str());
            image->hash = Common::R_HashString(image->name, 0);
            image->delayLoadPixels = true;

            image->noPicmip = !texture->HasMipMaps();
            image->width = static_cast<uint16_t>(texture->GetWidth());
            image->height = static_cast<uint16_t>(texture->GetHeight());
            image->depth = static_cast<uint16_t>(texture->GetDepth());

             if (texture->GetTextureType() == TextureType::T_2D)
                image->mapType = 3;
            else if (texture->GetTextureType() == TextureType::T_3D)
                image->mapType = 4;
            else if (texture->GetTextureType() == TextureType::T_CUBE)
                image->mapType = 5;
            else
                _ASSERT(false);

            //image->streaming = 1;
            //image->streamedParts[0].levelCount = 1;
            //image->streamedParts[0].levelSize = static_cast<uint32_t>(fileSize);
            //image->streamedParts[0].hash = dataHash & 0x1FFFFFFF;
            //image->streamedPartCount = 1;

            int mipMapCount = texture->HasMipMaps() ? texture->GetMipMapCount() : 1;
            size_t textureSize = 0;
            for (int previousMipLevel = 0; previousMipLevel < mipMapCount; previousMipLevel++)
            {
                textureSize += texture->GetSizeOfMipLevel(previousMipLevel) * texture->GetFaceCount();
            }

            image->streaming = 0;
            image->texture.loadDef = (GfxImageLoadDef*)malloc(sizeof(GfxImageLoadDef) + textureSize);
            memset(image->texture.loadDef, 0, sizeof(GfxImageLoadDef) + textureSize);
            image->texture.loadDef->format = texture->GetFormat()->GetDxgiFormat();
            image->texture.loadDef->levelCount = 1;
            image->texture.loadDef->resourceSize = textureSize;
            memcpy(image->texture.loadDef->data,
                   texture->GetBufferForMipLevel(0),
                   textureSize); // GetBufferForMipLevel(0) returns a pointer to the start of the image

            image->texture.loadDef->flags = 0;
            if (image->noPicmip)
                image->texture.loadDef->flags |= iwi27::IMG_FLAG_NOMIPMAPS;

            if (texture->GetTextureType() == TextureType::T_3D)
                image->texture.loadDef->flags |= iwi27::IMG_FLAG_VOLMAP;

            if (texture->GetTextureType() == TextureType::T_CUBE)
                image->texture.loadDef->flags |= iwi27::IMG_FLAG_CUBEMAP;

            return AssetCreationResult::Success(context.AddAsset<AssetImage>(assetName, image));
        }

    private:
        MemoryManager& m_memory;
        ISearchPath& m_search_path;
    };
} // namespace

namespace T6
{
    std::unique_ptr<AssetCreator<AssetImage>> CreateImageLoader(MemoryManager& memory, ISearchPath& searchPath)
    {
        return std::make_unique<ImageLoader>(memory, searchPath);
    }
} // namespace T6
