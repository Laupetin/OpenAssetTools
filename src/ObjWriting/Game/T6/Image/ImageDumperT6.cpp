#include "ImageDumperT6.h"

#include "Image/DdsWriter.h"
#include "Image/Dx12TextureLoader.h"
#include "Image/ImageCommon.h"
#include "Image/IwiLoader.h"
#include "Image/IwiWriter27.h"
#include "ObjContainer/IPak/IPak.h"
#include "ObjWriting.h"

#include <algorithm>
#include <cassert>
#include <format>

using namespace T6;
using namespace ::image;

namespace
{
    std::unique_ptr<Texture> LoadImageFromLoadDef(const GfxImage& image)
    {
        Dx12TextureLoader textureLoader;

        const auto& loadDef = *image.texture.loadDef;
        textureLoader.Width(image.width).Height(image.height).Depth(image.depth);

        if (loadDef.flags & iwi27::IMG_FLAG_VOLMAP)
            textureLoader.Type(TextureType::T_3D);
        else if (loadDef.flags & iwi27::IMG_FLAG_CUBEMAP)
            textureLoader.Type(TextureType::T_CUBE);
        else
            textureLoader.Type(TextureType::T_2D);

        textureLoader.Format(static_cast<oat::DXGI_FORMAT>(loadDef.format));
        textureLoader.HasMipMaps(!(loadDef.flags & iwi27::IMG_FLAG_NOMIPMAPS));
        return textureLoader.LoadTexture(loadDef.data);
    }

    std::unique_ptr<Texture> LoadImageFromIwi(const GfxImage& image, ISearchPath& searchPath)
    {
        if (image.streamedPartCount > 0)
        {
            for (auto* ipak : IIPak::Repository)
            {
                auto ipakStream = ipak->GetEntryStream(image.hash, image.streamedParts[0].hash);

                if (ipakStream)
                {
                    auto loadedTexture = iwi::LoadIwi(*ipakStream);
                    ipakStream->close();

                    if (loadedTexture != nullptr)
                        return loadedTexture;
                }
            }
        }

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

namespace T6::image
{
    Dumper::Dumper()
    {
        switch (ObjWriting::Configuration.ImageOutputFormat)
        {
        case ObjWriting::Configuration_t::ImageOutputFormat_e::DDS:
            m_writer = std::make_unique<DdsWriter>();
            break;
        case ObjWriting::Configuration_t::ImageOutputFormat_e::IWI:
            m_writer = std::make_unique<iwi27::IwiWriter>();
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
} // namespace T6::image
