#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/IW6/IW6.h"
#include "Image/ImageWriter.h"

#include <memory>

namespace image
{
    class DumperIW6 final : public AbstractAssetDumper<IW6::AssetImage>
    {
    public:
        DumperIW6();

    protected:
        void DumpAsset(AssetDumpingContext& context, const XAssetInfo<IW6::AssetImage::Type>& asset) override;

    private:
        std::unique_ptr<ImageWriter> m_writer;
    };
} // namespace image
