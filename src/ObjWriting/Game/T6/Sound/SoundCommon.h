#pragma once

#include "Game/T6/T6.h"

#include <string>

namespace T6::sound
{
    constexpr size_t FRAME_RATE_FOR_INDEX[]{
        8000,
        12000,
        16000,
        24000,
        32000,
        44100,
        48000,
        96000,
        192000,
    };

    constexpr const char* EXTENSION_BY_FORMAT[SND_ASSET_FORMAT_COUNT]{
        ".wav",  // SND_ASSET_FORMAT_PCMS16
        ".wav",  // SND_ASSET_FORMAT_PCMS24
        ".wav",  // SND_ASSET_FORMAT_PCMS32
        ".wav",  // SND_ASSET_FORMAT_IEEE
        ".xma",  // SND_ASSET_FORMAT_XMA4
        ".mp3",  // SND_ASSET_FORMAT_MP3
        ".wav",  // SND_ASSET_FORMAT_MSADPCM
        ".wma",  // SND_ASSET_FORMAT_WMA
        ".flac", // SND_ASSET_FORMAT_FLAC
        ".wav",  // SND_ASSET_FORMAT_WIIUADPCM
        ".mpc",  // SND_ASSET_FORMAT_MPC
    };

    _NODISCARD std::string GetAssetFilename(const std::string& basePath, std::string outputFileName, const std::string& extension);
}