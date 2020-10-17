#pragma once

#include "Loading/IZoneLoaderFactory.h"
#include <string>

namespace IW4
{
    class ZoneLoaderFactory final : public IZoneLoaderFactory
    {
        static const std::string MAGIC_SIGNED_INFINITY_WARD;
        static const std::string MAGIC_UNSIGNED;
        static const int VERSION;

        static const std::string MAGIC_AUTH_HEADER;
        static const uint8_t RSA_PUBLIC_KEY_INFINITY_WARD[];

        static const size_t AUTHED_CHUNK_SIZE;
        static const unsigned AUTHED_CHUNK_COUNT_PER_GROUP;

        static const int OFFSET_BLOCK_BIT_COUNT;
        static const block_t INSERT_BLOCK;

        class Impl;

    public:
        ZoneLoader* CreateLoaderForHeader(ZoneHeader& header, std::string& fileName) override;
    };
}
