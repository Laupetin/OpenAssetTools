#include "AssetDumperGfxImage.h"
#include "Image/IwiWriter27.h"
#include "ObjWriting.h"

using namespace T6;

bool AssetDumperGfxImage::ShouldDump(GfxImage* asset)
{
    return asset->loadedSize > 0;
}

std::string AssetDumperGfxImage::GetFileNameForAsset(Zone* zone, GfxImage* asset)
{
    return "images/" + std::string(asset->name) + ".iwi";
}

void AssetDumperGfxImage::DumpAsset(Zone* zone, GfxImage* asset, FileAPI::File* out)
{
    switch(ObjWriting::Configuration.ImageOutputFormat)
    {
    case ObjWriting::Configuration_t::ImageOutputFormat_e::DDS:
        // TODO this is not yet supported
        break;
    case ObjWriting::Configuration_t::ImageOutputFormat_e::IWI:
        IwiWriter27 writer;
        writer.DumpImage(out, asset->texture.texture);
        break;
    }
}