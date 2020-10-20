#include "AssetDumperGfxImage.h"

#include <cassert>

#include "ObjWriting.h"
#include "Image/IwiWriter8.h"
#include "Image/DdsWriter.h"

using namespace IW4;

AssetDumperGfxImage::AssetDumperGfxImage()
{
    switch (ObjWriting::Configuration.ImageOutputFormat)
    {
    case ObjWriting::Configuration_t::ImageOutputFormat_e::DDS:
        m_writer = new DdsWriter();
        break;
    case ObjWriting::Configuration_t::ImageOutputFormat_e::IWI:
        m_writer = new iwi8::IwiWriter();
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
    return image->cardMemory.platform[0] > 0;
}

std::string AssetDumperGfxImage::GetFileNameForAsset(Zone* zone, XAssetInfo<GfxImage>* asset)
{
    return "images/" + asset->m_name + m_writer->GetFileExtension();
}

void AssetDumperGfxImage::DumpAsset(Zone* zone, XAssetInfo<GfxImage>* asset, FileAPI::File* out)
{
    const auto* image = asset->Asset();
    m_writer->DumpImage(out, image->texture.texture);
}
