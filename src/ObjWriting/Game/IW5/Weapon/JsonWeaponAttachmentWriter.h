#pragma once

#include "Dumping/AssetDumpingContext.h"
#include "Game/IW5/IW5.h"

#include <ostream>

namespace IW5
{
    void DumpWeaponAttachmentAsJson(std::ostream& stream, const WeaponAttachment* attachment, AssetDumpingContext& context);
} // namespace IW5
