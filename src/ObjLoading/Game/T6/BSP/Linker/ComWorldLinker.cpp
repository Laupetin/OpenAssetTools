#include "ComWorldLinker.h"

#include "../BSPUtil.h"

#include <numbers>

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
        lightDef2d->name = m_memory.Dup("white_light");
        lightDef2d->lmapLookupStart = 0;            // always 0
        lightDef2d->attenuation.samplerState = 115; // always 115
        auto image2dAsset = m_context.LoadDependency<T6::AssetImage>("whitesquare");
        if (image2dAsset == nullptr)
            return false;
        lightDef2d->attenuation.image = image2dAsset->Asset();
        m_context.AddAsset<T6::AssetLightDef>(lightDef2d->name, lightDef2d);

        return true;
    }

    ComWorld* ComWorldLinker::linkComWorld(BSPData* bsp)
    {
        // all lights that aren't the sunlight or default light need their own GfxLightDef asset
        ComWorld* comWorld = m_memory.Alloc<ComWorld>();
        comWorld->name = m_memory.Dup(bsp->bspName.c_str());
        comWorld->isInUse = 1;

        // first two lights are the empty light and the sun light.
        size_t totalLightCount = bsp->lights.size() + BSPGameConstants::BSP_DEFAULT_LIGHT_COUNT;
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
            if (lightIdx == BSPGameConstants::EMPTY_LIGHT_INDEX)
                continue; // first (empty) light has no data
            else if (lightIdx == BSPGameConstants::SUN_LIGHT_INDEX)
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
                BSPLight* bspLight = &bsp->lights.at(lightIdx - BSPGameConstants::BSP_DEFAULT_LIGHT_COUNT);

                light->aAbB.x = 0.28173786401748657f;
                light->aAbB.y = 0.5634757280349731f;
                light->aAbB.z = 0.5634757280349731f;
                light->aAbB.w = 0.7071067690849304f;

                light->angle.x = 0.0f;
                light->angle.y = 0.0f;
                light->angle.z = -0.5235987901687622f;
                light->angle.w = 0.0f;

                light->canUseShadowMap = 1;

                light->cookieControl0.x = 0.0f;
                light->cookieControl0.y = 0.0f;
                light->cookieControl0.z = 1.0f;
                light->cookieControl0.w = 1.0f;
                light->cookieControl1.x = 0.0f;
                light->cookieControl1.y = 0.0f;
                light->cookieControl1.z = 0.0f;
                light->cookieControl1.w = 0.0f;
                light->cookieControl2.x = 0.0f;
                light->cookieControl2.y = 0.0f;
                light->cookieControl2.z = 0.0f;
                light->cookieControl2.w = 0.0f;

                light->color.x = bspLight->colour.x;
                light->color.y = bspLight->colour.y;
                light->color.z = bspLight->colour.z;
                light->cosHalfFovInner = cosf(bspLight->innerConeAngle);
                light->cosHalfFovOuter = cosf(bspLight->outerConeAngle);
                light->cosHalfFovExpanded = cosf(bspLight->outerConeAngle);

                light->cullDist = 1000;
                light->dAttenuation = 10000.0f;
                light->defName = "white_light";

                light->diffuseColor.x = bspLight->colour.x;
                light->diffuseColor.y = bspLight->colour.y;
                light->diffuseColor.z = bspLight->colour.z;
                light->diffuseColor.w = 0.0f;

                light->dir.x = bspLight->direction.x;
                light->dir.y = bspLight->direction.y;
                light->dir.z = bspLight->direction.z;

                light->exponent = 0;

                light->falloff.y = bspLight->range;

                light->mipDistance = bspLight->range;

                light->priority = 0;

                light->origin.x = bspLight->pos.x;
                light->origin.y = bspLight->pos.y;
                light->origin.z = bspLight->pos.z;

                light->radius = bspLight->range;
                light->rotationLimit = 1.0f;
                light->shadowmapVolume = 0;
                light->translationLimit = 0.0f;
                light->useCookie = 0;

                light->roundness = 0.0f;

                light->type = GFX_LIGHT_TYPE_SPOT;
            }
        }

        return comWorld;
    }
} // namespace BSP
