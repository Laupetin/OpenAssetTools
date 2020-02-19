#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T6/T6.h"
#include "Image/IImageWriter.h"

class AssetDumperGfxImage final : public AbstractAssetDumper<T6::GfxImage>
{
    IImageWriter* m_writer;

protected:
    bool ShouldDump(T6::GfxImage* asset) override;
    std::string GetFileNameForAsset(Zone* zone, T6::GfxImage* asset) override;
    void DumpAsset(Zone* zone, T6::GfxImage* asset, FileAPI::File* out) override;

public:
    AssetDumperGfxImage();
    ~AssetDumperGfxImage();

    AssetDumperGfxImage(const AssetDumperGfxImage& other) = delete;
    AssetDumperGfxImage(AssetDumperGfxImage&& other) noexcept = delete;
    AssetDumperGfxImage& operator=(const AssetDumperGfxImage& other) = delete;
    AssetDumperGfxImage& operator=(AssetDumperGfxImage&& other) noexcept = delete;
};