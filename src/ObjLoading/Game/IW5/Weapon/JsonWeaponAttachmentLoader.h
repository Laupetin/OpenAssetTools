#pragma once

#include "AssetLoading/IAssetLoadingManager.h"
#include "Game/IW5/IW5.h"
#include "Utils/MemoryManager.h"

#include <istream>

namespace IW5
{
    bool LoadWeaponAttachmentAsJson(std::istream& stream,
                                    WeaponAttachment& attachment,
                                    MemoryManager* memory,
                                    IAssetLoadingManager* manager,
                                    std::vector<XAssetInfoGeneric*>& dependencies,
                                    std::vector<IndirectAssetReference>& indirectAssetReferences);
} // namespace IW5
