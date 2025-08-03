#include "ImageDumperIW3.h"

#include "Image/DdsWriter.h"
#include "Image/Dx9TextureLoader.h"
#include "Image/ImageCommon.h"
#include "Image/IwiLoader.h"
#include "Image/IwiTypes.h"
#include "Image/IwiWriter6.h"
#include "Image/Texture.h"
#include "ObjWriting.h"
#include "SearchPath/ISearchPath.h"

#include <algorithm>
#include <cassert>
#include <format>

using namespace IW3;
using namespace ::image;

namespace
{
    std::unique_ptr<Texture> LoadImageFromLoadDef(const GfxImage& image)
    {
        Dx9TextureLoader textureLoader;

        const auto& loadDef = *image.texture.loadDef;
        textureLoader.Width(loadDef.dimensions[0]).Height(loadDef.dimensions[1]).Depth(loadDef.dimensions[2]);

        if (loadDef.flags & iwi6::IMG_FLAG_VOLMAP)
            textureLoader.Type(TextureType::T_3D);
        else if (loadDef.flags & iwi6::IMG_FLAG_CUBEMAP)
            textureLoader.Type(TextureType::T_CUBE);
        else
            textureLoader.Type(TextureType::T_2D);

        textureLoader.Format(static_cast<oat::D3DFORMAT>(loadDef.format));
        textureLoader.HasMipMaps(!(loadDef.flags & iwi6::IMG_FLAG_NOMIPMAPS));
        return textureLoader.LoadTexture(loadDef.data);
    }

    std::unique_ptr<Texture> LoadImageFromIwi(const GfxImage& image, ISearchPath& searchPath)
    {
        const auto imageFileName = std::format("images/{}.iwi", image.name);
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

namespace IW3::image
{
    Dumper::Dumper()
    {
        switch (ObjWriting::Configuration.ImageOutputFormat)
        {
        case ObjWriting::Configuration_t::ImageOutputFormat_e::DDS:
            m_writer = std::make_unique<DdsWriter>();
            break;
        case ObjWriting::Configuration_t::ImageOutputFormat_e::IWI:
            m_writer = std::make_unique<iwi6::IwiWriter>();
            break;
        default:
            assert(false);
            m_writer = nullptr;
            break;
        }
    }

    bool Dumper::ShouldDump(XAssetInfo<GfxImage>* asset)
    {
        return true;
    }

    void Dumper::DumpAsset(AssetDumpingContext& context, XAssetInfo<GfxImage>* asset)
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
} // namespace IW3::image
