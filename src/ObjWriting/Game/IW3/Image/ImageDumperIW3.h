#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW3/IW3.h"
#include "Image/IImageWriter.h"

#include <memory>

namespace image
{
    class DumperIW3 final : public AbstractAssetDumper<IW3::GfxImage>
    {
    public:
        DumperIW3();

    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<IW3::GfxImage>& asset) override;

    private:
        std::unique_ptr<IImageWriter> m_writer;
    };
} // namespace image
