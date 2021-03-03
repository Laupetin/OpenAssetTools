#include "AssetDumperGfxImage.h"

#include <cassert>

#include "ObjWriting.h"
#include "Image/IwiWriter27.h"
#include "Image/DdsWriter.h"

using namespace T6;

AssetDumperGfxImage::AssetDumperGfxImage()
{
    switch (ObjWriting::Configuration.ImageOutputFormat)
    {
    case ObjWriting::Configuration_t::ImageOutputFormat_e::DDS:
        m_writer = new DdsWriter();
        break;
    case ObjWriting::Configuration_t::ImageOutputFormat_e::IWI:
        m_writer = new iwi27::IwiWriter();
        break;
    default:
        assert(false);
        m_writer = nullptr;
        break;
    }
}

AssetDumperGfxImage::~AssetDumperGfxImage()
{
    delete m_writer;
    m_writer = nullptr;
}

bool AssetDumperGfxImage::ShouldDump(XAssetInfo<GfxImage>* asset)
{
    const auto* image = asset->Asset();
    return image->loadedSize > 0;
}

std::string AssetDumperGfxImage::GetFileNameForAsset(Zone* zone, XAssetInfo<GfxImage>* asset)
{
    return "images/" + asset->m_name + m_writer->GetFileExtension();
}

void AssetDumperGfxImage::DumpAsset(Zone* zone, XAssetInfo<GfxImage>* asset, std::ostream& stream)
{
    const auto* image = asset->Asset();
    m_writer->DumpImage(stream, image->texture.texture);
}
