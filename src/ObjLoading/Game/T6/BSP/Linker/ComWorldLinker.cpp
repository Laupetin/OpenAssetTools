#include "ComWorldLinker.h"

namespace BSP
{
    ComWorldLinker::ComWorldLinker(MemoryManager& memory, ISearchPath& searchPath, AssetCreationContext& context)
        : m_memory(memory),
        m_search_path(searchPath),
        m_context(context)
    {
    }

    AssetCreationResult ComWorldLinker::linkComWorld(BSPData* bsp)
    {
        // all lights that aren't the sunlight or default light need their own GfxLightDef asset
        ComWorld* comWorld = m_memory.Alloc<ComWorld>();
        comWorld->name = m_memory.Dup(bsp->bspName.c_str());
        comWorld->isInUse = 1;
        comWorld->primaryLightCount = 2;
        comWorld->primaryLights = m_memory.Alloc<ComPrimaryLight>(comWorld->primaryLightCount);

        // static light is always empty
        ComPrimaryLight* staticLight = &comWorld->primaryLights[0];
        memset(staticLight, 0, sizeof(ComPrimaryLight));

        ComPrimaryLight* sunLight = &comWorld->primaryLights[1];
        memset(sunLight, 0, sizeof(ComPrimaryLight));
        sunLight->type = 1;
        sunLight->diffuseColor.r = 0.75f;
        sunLight->diffuseColor.g = 0.75f;
        sunLight->diffuseColor.b = 0.75f;
        sunLight->diffuseColor.a = 1.0f;
        sunLight->dir.x = 0.0f;
        sunLight->dir.y = 0.0f;
        sunLight->dir.z = 0.0f;

        auto comWorldAsset = m_context.AddAsset<AssetComWorld>(comWorld->name, comWorld);
        return AssetCreationResult::Success(comWorldAsset);
    }
}

