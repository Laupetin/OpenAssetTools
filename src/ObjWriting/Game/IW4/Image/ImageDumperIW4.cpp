#include "ImageDumperIW4.h"

#include "Image/DdsWriter.h"
#include "Image/Dx9TextureLoader.h"
#include "Image/ImageCommon.h"
#include "Image/IwiLoader.h"
#include "Image/IwiWriter8.h"
#include "ObjWriting.h"
#include "Utils/Logging/Log.h"

#include <algorithm>
#include <cassert>
#include <format>

using namespace IW4;
using namespace image;

namespace
{
    std::unique_ptr<Texture> LoadImageFromLoadDef(const GfxImage& image)
    {
        Dx9TextureLoader textureLoader;

        const auto& loadDef = *image.texture.loadDef;
        textureLoader.Width(image.width).Height(image.height).Depth(image.depth);

        if ((loadDef.flags & image::iwi8::IMG_FLAG_MAPTYPE_MASK) == image::iwi8::IMG_FLAG_MAPTYPE_3D)
            textureLoader.Type(TextureType::T_3D);
        else if ((loadDef.flags & image::iwi8::IMG_FLAG_MAPTYPE_MASK) == image::iwi8::IMG_FLAG_MAPTYPE_CUBE)
            textureLoader.Type(TextureType::T_CUBE);
        else
            textureLoader.Type(TextureType::T_2D);

        textureLoader.Format(static_cast<oat::D3DFORMAT>(loadDef.format));
        textureLoader.HasMipMaps(!(loadDef.flags & image::iwi8::IMG_FLAG_NOMIPMAPS));
        return textureLoader.LoadTexture(loadDef.data);
    }

    std::unique_ptr<Texture> LoadImageFromIwi(const GfxImage& image, ISearchPath& searchPath)
    {
        const auto imageFileName = image::GetFileNameForAsset(image.name, ".iwi");
        const auto filePathImage = searchPath.Open(imageFileName);
        if (!filePathImage.IsOpen())
        {
            con::error("Could not find data for image \"{}\"", image.name);
            return nullptr;
        }

        return image::LoadIwi(*filePathImage.m_stream);
    }

    std::unique_ptr<Texture> LoadImageData(ISearchPath& searchPath, const GfxImage& image)
    {
        if (image.texture.loadDef && image.texture.loadDef->resourceSize > 0)
            return LoadImageFromLoadDef(image);

        return LoadImageFromIwi(image, searchPath);
    }
} // namespace

namespace image
{
    DumperIW4::DumperIW4(const AssetPool<AssetImage::Type>& pool)
        : AbstractAssetDumper(pool)
    {
        switch (ObjWriting::Configuration.ImageOutputFormat)
        {
        case ObjWriting::Configuration_t::ImageOutputFormat_e::DDS:
            m_writer = std::make_unique<DdsWriter>();
            break;
        case ObjWriting::Configuration_t::ImageOutputFormat_e::IWI:
            m_writer = std::make_unique<iwi8::IwiWriter>();
            break;
        default:
            assert(false);
            m_writer = nullptr;
            break;
        }
    }

    void DumperIW4::DumpAsset(AssetDumpingContext& context, const XAssetInfo<AssetImage::Type>& asset)
    {
        const auto* image = asset.Asset();
        const auto texture = LoadImageData(context.m_obj_search_path, *image);
        if (!texture)
            return;

        const auto assetFile = context.OpenAssetFile(GetFileNameForAsset(asset.m_name, m_writer->GetFileExtension()));

        if (!assetFile)
            return;

        auto& stream = *assetFile;
        m_writer->DumpImage(stream, texture.get());
    }
} // namespace image
