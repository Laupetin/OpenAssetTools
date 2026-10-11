#pragma once

#include "XAnim/XAnimCommon.h"
#include "XModel/Gltf/GltfOutput.h"

#include <string>

namespace xanim_gltf
{
    void Write(const xanim::CommonXAnimParts& parts, const std::string& name, const gltf::Output& output);
}
