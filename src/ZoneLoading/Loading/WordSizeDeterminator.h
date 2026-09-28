#pragma once

#include "Game/GameVariant.h"
#include "ZoneLoader.h"

#include <optional>

struct XAssetListOffsets
{
    size_t offsetStringCount;
    size_t offsetStringPointer;
    size_t offsetAssetCount;
    size_t offsetAssetPointer;
};

std::optional<GameWordSize> DetermineWordSizeFromXAssetList(std::istream& stream,
                                                            std::unique_ptr<ZoneReader> zoneReader,
                                                            const XAssetListOffsets& offsets32,
                                                            const XAssetListOffsets& offsets64);
