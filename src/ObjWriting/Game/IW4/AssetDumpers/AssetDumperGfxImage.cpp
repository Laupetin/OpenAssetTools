#include "AssetDumperGfxImage.h"

#include <cassert>

#include "ObjWriting.h"
#include "Image/IwiWriter27.h"
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
        m_writer = new IwiWriter27();
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

bool AssetDumperGfxImage::ShouldDump(GfxImage* asset)
{
    return asset->cardMemory.platform[0] > 0;
}

std::string AssetDumperGfxImage::GetFileNameForAsset(Zone* zone, GfxImage* asset)
{
    return "images/" + std::string(asset->name) + m_writer->GetFileExtension();
}

void AssetDumperGfxImage::DumpAsset(Zone* zone, GfxImage* asset, FileAPI::File* out)
{
    m_writer->DumpImage(out, asset->texture.texture);
}
