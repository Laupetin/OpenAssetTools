#pragma once

#include "Asset/AssetCreationContext.h"
#include "Asset/AssetRegistration.h"
#include "Game/IW5/IW5.h"
#include "Utils/MemoryManager.h"

#include <istream>

namespace IW5
{
    bool LoadWeaponAttachmentAsJson(std::istream& stream,
                                    WeaponAttachment& attachment,
                                    MemoryManager& memory,
                                    AssetCreationContext& context,
                                    AssetRegistration<AssetAttachment>& registration);
} // namespace IW5
