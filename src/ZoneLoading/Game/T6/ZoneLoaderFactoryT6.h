#pragma once

#include "Loading/IZoneLoaderFactory.h"
#include <string>

class ZoneLoaderFactoryT6 final : public IZoneLoaderFactory
{
    static const std::string MAGIC_SIGNED_TREYARCH;
    static const std::string MAGIC_SIGNED_ASSET_BUILDER;
    static const std::string MAGIC_UNSIGNED;
    static const std::string MAGIC_UNSIGNED_SERVER;
    static const int VERSION;

    static const int STREAM_COUNT;
    static const int XCHUNK_SIZE;
    static const int VANILLA_BUFFER_SIZE;
    static const int OFFSET_BLOCK_BIT_COUNT;
    static const block_t INSERT_BLOCK;

    static const std::string MAGIC_AUTH_HEADER;
    static const uint8_t SALSA20_KEY_TREYARCH[];
    static const uint8_t RSA_PUBLIC_KEY_TREYARCH[];

    class ZoneLoaderFactoryT6Impl;

public:
    ZoneLoader* CreateLoaderForHeader(ZoneHeader& header, std::string& fileName) override;
};
