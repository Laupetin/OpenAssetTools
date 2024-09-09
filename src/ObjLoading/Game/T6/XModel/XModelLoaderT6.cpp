#include "XModelLoaderT6.h"

#include "Game/T6/CommonT6.h"
#include "Game/T6/Json/JsonXModel.h"

#define GAME_NAMESPACE T6

namespace T6
{
    const char* HITLOC_NAMES[]{
        // clang-format off
        "none",
        "helmet",
        "head",
        "neck",
        "torso_upper",
        "torso_middle",
        "torso_lower",
        "right_arm_upper",
        "left_arm_upper",
        "right_arm_lower",
        "left_arm_lower",
        "right_hand",
        "left_hand",
        "right_leg_upper",
        "left_leg_upper",
        "right_leg_lower",
        "left_leg_lower",
        "right_foot",
        "left_foot",
        "gun",
        "shield",
        // clang-format on
    };
    static_assert(std::extent_v<decltype(HITLOC_NAMES)> == HITLOC_COUNT);
} // namespace T6

#include "XModel/GenericXModelLoader.inc.h"

namespace T6
{
    bool LoadXModel(std::istream& stream, XModel& xmodel, MemoryManager* memory, IAssetLoadingManager* manager, std::vector<XAssetInfoGeneric*>& dependencies)
    {
        std::set<XAssetInfoGeneric*> dependenciesSet;
        XModelLoader loader(stream, *memory, *manager, dependenciesSet);

        dependencies.assign(dependenciesSet.cbegin(), dependenciesSet.cend());

        return loader.Load(xmodel);
    }
} // namespace T6
