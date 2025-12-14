#include "ComWorldLinker.h"

using namespace T6;

namespace BSP
{
    ComWorldLinker::ComWorldLinker(MemoryManager& memory, ISearchPath& searchPath, AssetCreationContext& context)
        : m_memory(memory),
          m_search_path(searchPath),
          m_context(context)
    {
    }

    ComWorld* ComWorldLinker::LinkComWorld(const BSPData& bsp) const
    {
        // all lights that aren't the sunlight or default light need their own GfxLightDef asset
        ComWorld* comWorld = m_memory.Alloc<ComWorld>();

        comWorld->name = m_memory.Dup(bsp.bspName.c_str());
        comWorld->isInUse = 1;
        comWorld->primaryLightCount = BSPGameConstants::BSP_DEFAULT_LIGHT_COUNT;
        comWorld->primaryLights = m_memory.Alloc<ComPrimaryLight>(comWorld->primaryLightCount);

        // first (static) light is always empty

        ComPrimaryLight* sunLight = &comWorld->primaryLights[1];
        const vec4_t sunLightColor = BSPEditableConstants::SUNLIGHT_COLOR;
        const vec3_t sunLightDirection = BSPEditableConstants::SUNLIGHT_DIRECTION;

        sunLight->type = GFX_LIGHT_TYPE_DIR;
        sunLight->diffuseColor.r = sunLightColor.r;
        sunLight->diffuseColor.g = sunLightColor.g;
        sunLight->diffuseColor.b = sunLightColor.b;
        sunLight->diffuseColor.a = sunLightColor.a;
        sunLight->dir.x = sunLightDirection.x;
        sunLight->dir.y = sunLightDirection.y;
        sunLight->dir.z = sunLightDirection.z;

        return comWorld;
    }
} // namespace BSP
