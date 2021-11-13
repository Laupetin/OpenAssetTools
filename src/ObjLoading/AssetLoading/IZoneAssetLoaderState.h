#pragma once

class IZoneAssetLoaderState
{
protected:
    IZoneAssetLoaderState() = default;

public:
    virtual ~IZoneAssetLoaderState() = default;
    IZoneAssetLoaderState(const IZoneAssetLoaderState& other) = default;
    IZoneAssetLoaderState(IZoneAssetLoaderState&& other) noexcept = default;
    IZoneAssetLoaderState& operator=(const IZoneAssetLoaderState& other) = default;
    IZoneAssetLoaderState& operator=(IZoneAssetLoaderState&& other) noexcept = default;
};