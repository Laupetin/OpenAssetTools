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

bool AssetDumperGfxImage::ShouldDump(XAssetInfo<GfxImage>* asset)
{
    const auto* image = asset->Asset();
    return image->cardMemory.platform[0] > 0;
}

bool AssetDumperGfxImage::CanDumpAsRaw()
{
    return true;
}

std::string AssetDumperGfxImage::GetFileNameForAsset(Zone* zone, XAssetInfo<GfxImage>* asset)
{
    std::string cleanAssetName = asset->m_name;
    for (auto& c : cleanAssetName)
    {
        switch (c)
        {
        case '*':
            c = '_';
            break;

        default:
            break;
        }
    }

    return "images/" + cleanAssetName + m_writer->GetFileExtension();
}

void AssetDumperGfxImage::DumpRaw(AssetDumpingContext& context, XAssetInfo<GfxImage>* asset, std::ostream& stream)
{
    const auto* image = asset->Asset();
    m_writer->DumpImage(stream, image->texture.texture);
}
