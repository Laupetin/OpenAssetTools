#include "LoaderImageIW5.h"

#include "Game/IW5/IW5.h"
#include "Image/ImageCommon.h"
#include "Image/IwiLoader.h"
#include "Utils/Logging/Log.h"

#include <cstring>
#include <format>
#include <iostream>
#include <sstream>

using namespace IW5;

namespace
{
    constexpr auto MAX_IMAGE_NAME_SIZE = 0x800;

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
            const auto fileName = image::GetFileNameForAsset(assetName, ".iwi");
            const auto file = m_search_path.Open(fileName);
            if (!file.IsOpen())
                return AssetCreationResult::NoAction();

            const auto fileSize = static_cast<size_t>(file.m_length);
            const auto fileData = std::make_unique<char[]>(fileSize);
            file.m_stream->read(fileData.get(), fileSize);

            std::istringstream ss(std::string(fileData.get(), fileSize));
            const auto texture = iwi::LoadIwi(ss);
            if (!texture)
            {
                con::error("Failed to load texture from: {}", fileName);
                return AssetCreationResult::Failure();
            }

            auto* image = m_memory.Alloc<GfxImage>();
            image->name = m_memory.Dup(assetName.c_str());
            image->noPicmip = !texture->HasMipMaps();
            image->width = static_cast<uint16_t>(texture->GetWidth());
            image->height = static_cast<uint16_t>(texture->GetHeight());
            image->depth = static_cast<uint16_t>(texture->GetDepth());

            image->texture.loadDef = m_memory.Alloc<GfxImageLoadDef>();

            return AssetCreationResult::Success(context.AddAsset<AssetImage>(assetName, image));
        }

    private:
        MemoryManager& m_memory;
        ISearchPath& m_search_path;
    };
} // namespace

namespace image
{
    std::unique_ptr<AssetCreator<AssetImage>> CreateLoaderIW5(MemoryManager& memory, ISearchPath& searchPath)
    {
        return std::make_unique<ImageLoader>(memory, searchPath);
    }
} // namespace image
