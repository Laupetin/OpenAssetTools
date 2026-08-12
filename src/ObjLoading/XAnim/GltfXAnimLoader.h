#pragma once

#include "XAnim/XAnimCommon.h"
#include "XModel/Gltf/GltfInput.h"

#include <expected>
#include <memory>
#include <string>

namespace xanim_gltf
{
    std::expected<std::unique_ptr<xanim::CommonXAnimParts>, std::string> Load(const gltf::Input& input);
}
