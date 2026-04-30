#include "ComWorldLinker.h"

#include <numbers>

namespace
{
    constexpr const char* DEFAULT_LIGHTDEF_NAME = "white_light";
    constexpr short LIGHT_CULLDIST = 10000;
} // namespace

namespace BSP
{
    ComWorldLinker::ComWorldLinker(MemoryManager& memory, ISearchPath& searchPath, AssetCreationContext& context)
        : m_memory(memory),
          m_search_path(searchPath),
          m_context(context)
    {
    }

    bool ComWorldLinker::createLightDefs()
    {
        T6::GfxLightDef* lightDef2d = m_memory.Alloc<T6::GfxLightDef>();
        lightDef2d->name = m_memory.Dup(DEFAULT_LIGHTDEF_NAME);
        lightDef2d->lmapLookupStart = 0;            // always 0
        lightDef2d->attenuation.samplerState = 115; // always 115
        auto image2dAsset = m_context.LoadDependency<T6::AssetImage>(",$white");
        if (image2dAsset == nullptr)
            return false;
        lightDef2d->attenuation.image = image2dAsset->Asset();
        m_context.AddAsset<T6::AssetLightDef>(lightDef2d->name, lightDef2d);

        return true;
    }

    ComWorld* ComWorldLinker::linkComWorld(BSPData* bsp)
    {
        ComWorld* comWorld = m_memory.Alloc<ComWorld>();
        comWorld->name = m_memory.Dup(bsp->bspName.c_str());
        comWorld->isInUse = 1;

        // first two lights are the empty light and the sun light.
        size_t totalLightCount = bsp->lights.size() + BSP_DEFAULT_LIGHT_COUNT;
        comWorld->primaryLightCount = static_cast<unsigned int>(totalLightCount);
        comWorld->primaryLights = m_memory.Alloc<ComPrimaryLight>(totalLightCount);

        if (!createLightDefs())
        {
            con::error("Unable to create lightdef assets.");
            return nullptr;
        }

        for (size_t lightIdx = 0; lightIdx < totalLightCount; lightIdx++)
        {
            ComPrimaryLight* light = &comWorld->primaryLights[lightIdx];
            if (lightIdx == EMPTY_LIGHT_INDEX)
                continue; // first (empty) light has no data
            else if (lightIdx == SUN_LIGHT_INDEX)
            {
                BSPLight* bspLight = &bsp->sunlight;

                light->type = GFX_LIGHT_TYPE_DIR;
                light->dir.x = bspLight->direction.x;
                light->dir.y = bspLight->direction.y;
                light->dir.z = bspLight->direction.z;
                light->diffuseColor.x = bspLight->colour.x;
                light->diffuseColor.y = bspLight->colour.y;
                light->diffuseColor.z = bspLight->colour.z;
                light->color.x = bspLight->colour.x;
                light->color.y = bspLight->colour.y;
                light->color.z = bspLight->colour.z;
            }
            else
            {
                BSPLight* bspLight = &bsp->lights.at(lightIdx - BSP_DEFAULT_LIGHT_COUNT);

                light->type = GFX_LIGHT_TYPE_SPOT;

                light->color.x = bspLight->colour.x;
                light->color.y = bspLight->colour.y;
                light->color.z = bspLight->colour.z;
                light->diffuseColor.x = bspLight->colour.x;
                light->diffuseColor.y = bspLight->colour.y;
                light->diffuseColor.z = bspLight->colour.z;

                light->cosHalfFovInner = cosf(bspLight->innerConeAngle);
                light->cosHalfFovOuter = cosf(bspLight->outerConeAngle);
                light->cosHalfFovExpanded = cosf(bspLight->outerConeAngle);

                light->dir.x = bspLight->direction.x;
                light->dir.y = bspLight->direction.y;
                light->dir.z = bspLight->direction.z;

                light->falloff.y = bspLight->range;
                light->radius = bspLight->range;
                light->mipDistance = bspLight->range;

                light->origin.x = bspLight->pos.x;
                light->origin.y = bspLight->pos.y;
                light->origin.z = bspLight->pos.z;

                light->dAttenuation = bspLight->intensity;

                // I think AABB controls the lens shape of the light
                light->aAbB.x = 0.75f;
                light->aAbB.y = 1.0f;
                light->aAbB.z = 0.75f;
                light->aAbB.w = 1.0f;

                light->cullDist = LIGHT_CULLDIST;
                light->defName = DEFAULT_LIGHTDEF_NAME; // all lights that aren't the sunlight or default light need their own GfxLightDef asset
                light->rotationLimit = 1.0f;            // 1.0f - doesn't rotate, -1.0f - unclamped rotation
                light->translationLimit = 0.0f;         // 0.0f - doesn't translate, above 0.0f - distance per game update translated
                light->roundness = 1.0f;                // 0.0f - light is a square. 1.0f - light is a circle
                light->canUseShadowMap = 1;             // light does not show up with this set to 0
            }
        }

        return comWorld;
    }
} // namespace BSP
