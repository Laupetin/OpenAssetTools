#include "UnsupportedAssetTypeException.h"

UnsupportedAssetTypeException::UnsupportedAssetTypeException(const int assetType)
{
    m_asset_type = assetType;
}

std::string UnsupportedAssetTypeException::DetailedMessage()
{
    return "Zone has an unsupported asset type " + std::to_string(m_asset_type) + " and therefore cannot be loaded.";
}

char const* UnsupportedAssetTypeException::what() const noexcept
{
    return "Zone has unsupported asset type.";
}