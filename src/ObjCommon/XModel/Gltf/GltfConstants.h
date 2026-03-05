#pragma once

#include "Utils/FileUtils.h"

#include <cstdint>
#include <string>

namespace gltf
{
    constexpr uint32_t GLTF_MAGIC = utils::MakeMagic32('g', 'l', 'T', 'F');
    constexpr uint32_t GLTF_VERSION = 2u;
    constexpr auto GLTF_VERSION_STRING = "2.0";

    constexpr uint32_t CHUNK_MAGIC_JSON = utils::MakeMagic32('J', 'S', 'O', 'N');
    constexpr uint32_t CHUNK_MAGIC_BIN = utils::MakeMagic32('B', 'I', 'N', '\x00');

    constexpr auto GLTF_LENGTH_OFFSET = 8u;
    constexpr auto GLTF_JSON_CHUNK_LENGTH_OFFSET = 12u;
    constexpr auto GLTF_JSON_CHUNK_DATA_OFFSET = 20u;

    constexpr auto GLTF_DATA_URI_PREFIX = "data:application/octet-stream;base64,";
    constexpr auto URI_PREFIX_LENGTH = std::char_traits<char>::length(GLTF_DATA_URI_PREFIX);

    constexpr auto GLTF_ATTRIBUTE_POSITION = "POSITION";
    constexpr auto GLTF_ATTRIBUTE_NORMAL = "NORMAL";
    constexpr auto GLTF_ATTRIBUTE_TEXCOORD_0 = "TEXCOORD_0";
    constexpr auto GLTF_ATTRIBUTE_JOINTS_0 = "JOINTS_0";
    constexpr auto GLTF_ATTRIBUTE_WEIGHTS_0 = "WEIGHTS_0";
} // namespace gltf
