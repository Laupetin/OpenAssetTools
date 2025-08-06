#include "ImageDumperT5.h"

#include "Image/DdsWriter.h"
#include "Image/Dx9TextureLoader.h"
#include "Image/ImageCommon.h"
#include "Image/IwiLoader.h"
#include "Image/IwiWriter13.h"
#include "ObjWriting.h"

#include <algorithm>
#include <cassert>
#include <format>

using namespace T5;

namespace
{
    std::unique_ptr<Texture> LoadImageFromLoadDef(const GfxImage& image)
    {
        Dx9TextureLoader textureLoader;

        const auto& loadDef = *image.texture.loadDef;
        textureLoader.Width(image.width).Height(image.height).Depth(image.depth);

        if (loadDef.flags & iwi13::IMG_FLAG_VOLMAP)
            textureLoader.Type(TextureType::T_3D);
        else if (loadDef.flags & iwi13::IMG_FLAG_CUBEMAP)
            textureLoader.Type(TextureType::T_CUBE);
        else
            textureLoader.Type(TextureType::T_2D);

        textureLoader.Format(static_cast<oat::D3DFORMAT>(loadDef.format));
        textureLoader.HasMipMaps(!(loadDef.flags & iwi13::IMG_FLAG_NOMIPMAPS));
        return textureLoader.LoadTexture(loadDef.data);
    }

    std::unique_ptr<Texture> LoadImageFromIwi(const GfxImage& image, ISearchPath& searchPath)
    {
        const auto imageFileName = image::GetFileNameForAsset(image.name, ".iwi");
        const auto filePathImage = searchPath.Open(imageFileName);
        if (!filePathImage.IsOpen())
        {
            std::cerr << std::format("Could not find data for image \"{}\"\n", image.name);
            return nullptr;
        }

        return iwi::LoadIwi(*filePathImage.m_stream);
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
    DumperT5::DumperT5()
    {
        switch (ObjWriting::Configuration.ImageOutputFormat)
        {
        case ObjWriting::Configuration_t::ImageOutputFormat_e::DDS:
            m_writer = std::make_unique<DdsWriter>();
            break;
        case ObjWriting::Configuration_t::ImageOutputFormat_e::IWI:
            m_writer = std::make_unique<iwi13::IwiWriter>();
            break;
        default:
            assert(false);
            m_writer = nullptr;
            break;
        }
    }

    bool DumperT5::ShouldDump(XAssetInfo<GfxImage>* asset)
    {
        return true;
    }

    void DumperT5::DumpAsset(AssetDumpingContext& context, XAssetInfo<GfxImage>* asset)
    {
        const auto* image = asset->Asset();
        const auto texture = LoadImageData(context.m_obj_search_path, *image);
        if (!texture)
            return;

        const auto assetFile = context.OpenAssetFile(GetFileNameForAsset(asset->m_name, m_writer->GetFileExtension()));

        if (!assetFile)
            return;

        auto& stream = *assetFile;
        m_writer->DumpImage(stream, texture.get());
    }
} // namespace image
