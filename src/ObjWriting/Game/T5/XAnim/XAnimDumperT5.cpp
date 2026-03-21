#include "XAnimDumperT5.h"

#include "Utils/Logging/Log.h"

using namespace T5;

namespace xanim
{
    void DumperT5::DumpAsset(AssetDumpingContext& context, const XAssetInfo<AssetXAnim::Type>& asset)
    {
        auto* anim = asset.Asset();
        const auto& scriptStrings = context.m_zone.m_script_strings;

        con::info("Anim {}", anim->name);
        if (anim->names)
        {
            for (auto boneIndex = 0u; boneIndex < anim->boneCount[PART_TYPE_ALL]; boneIndex++)
            {
                const auto* boneName = scriptStrings.CValue(anim->names[boneIndex]);
                con::info("  Bone {}: {}", boneIndex, boneName ? boneName : "<nullptr>");
            }
        }

        con::info("");
    }
} // namespace xanim
