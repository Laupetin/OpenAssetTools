#pragma once
#include "LoadingException.h"

class UnsupportedAssetTypeException final : public LoadingException
{
    int m_asset_type;

public:
    explicit UnsupportedAssetTypeException(int assetType);

    std::string DetailedMessage() override;
    char const* what() const override;
};
