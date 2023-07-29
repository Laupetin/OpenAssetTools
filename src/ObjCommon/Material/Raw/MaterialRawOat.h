#pragma once

#include <cstdint>

namespace material
{
    struct MaterialRawOatHeader
    {
        uint8_t magic[3];
        uint8_t version;
    };

    struct MaterialInfoRaw
    {
        uint32_t nameOffset;
        uint8_t gameFlags;
        uint8_t sortKey;
        uint8_t textureAtlasRowCount;
        uint8_t textureAtlasColumnCount;
        uint32_t surfaceFlags;
        uint32_t contents;
    };

    struct MaterialRaw
    {
        MaterialInfoRaw info;
        uint32_t refStateBits[2];
        uint16_t textureCount;
        uint16_t constantCount;
        uint32_t techSetNameOffset;
        uint32_t textureTableOffset;
        uint32_t constantTableOffset;
    };

    struct MaterialTextureDefRaw
    {
        uint32_t nameOffset;
        uint8_t samplerState;
        uint8_t semantic;
        uint32_t imageNameOffset;
        uint32_t waterDefOffset;
    };

    struct MaterialWaterDefRaw
    {
        uint32_t textureWidth;
        float horizontalWorldLength;
        float verticalWorldLength;
        float amplitude;
        float windSpeed;
        float windDirection[2];
    };

    struct MaterialConstantDefRaw
    {
        uint32_t nameOffset;
        float literal[4];
    };
}