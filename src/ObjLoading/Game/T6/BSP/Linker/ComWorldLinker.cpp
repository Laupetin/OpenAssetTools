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
        T6::GfxLightDef* lightDefCube = m_memory.Alloc<T6::GfxLightDef>();
        lightDefCube->name = m_memory.Dup("white_light_cube");
        lightDefCube->lmapLookupStart = 0;            // always 0
        lightDefCube->attenuation.samplerState = 115; // always 115
        auto imageCubeAsset = m_context.LoadDependency<T6::AssetImage>("whitesquare_ft");
        if (imageCubeAsset == nullptr)
            return false;
        lightDefCube->attenuation.image = imageCubeAsset->Asset();
        m_context.AddAsset<T6::AssetLightDef>(lightDefCube->name, lightDefCube);

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

    // does not set: type, cosHalfFovOuter, cosHalfFovInner, cosHalfFovExpanded, roundness, defName
    void setLightCommonValues(ComPrimaryLight* light, BSPLight* bspLight)
    {
        light->origin.x = bspLight->pos.x;
        light->origin.y = bspLight->pos.y;
        light->origin.z = bspLight->pos.z;

        // colour also effects the brightness of the light, it's unclear what the min/max is
        vec3_t BSPColor;
        BSPColor.x = bspLight->colour.x;
        BSPColor.y = bspLight->colour.y;
        BSPColor.z = bspLight->colour.z;
        light->color.x = BSPColor.x;
        light->color.y = BSPColor.y;
        light->color.z = BSPColor.z;
        light->diffuseColor.x = BSPColor.x;
        light->diffuseColor.y = BSPColor.y;
        light->diffuseColor.z = BSPColor.z;
        light->diffuseColor.w = 0.0f; // always 0

        // the forward dir of the light, default is (0, 0, 1) to face down (-Z up)
        light->dir.x = bspLight->direction.x;
        light->dir.y = bspLight->direction.y;
        light->dir.z = bspLight->direction.z;

        light->dAttenuation = bspLight->intensity; // not 100% sure if these values are calculated the same

        // the radius of the light's lens
        light->radius = bspLight->range;

        light->falloff.x = 0.0f;
        light->falloff.y = bspLight->range; // only Y is set
        light->falloff.z = 0.0f;
        light->falloff.w = 0.0f;

        // allways 0
        light->angle.x = 0.0f;
        light->angle.y = 0.0f;
        light->angle.z = 0.0f;
        light->angle.w = 0.0f;

        // 0 - light cannot move
        // 1 <= x > 0 - limit that it can move each game update
        light->translationLimit = 0.0f;

        // 1.0f - light cannot rotate
        // -1.0f - infinitely rotate
        // between 1 and -1 -  limit that it can rotate each game update
        light->rotationLimit = 1.0f;

        // default values from official map
        light->useCookie = 0;
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

        // these are possibly the result of cos(), unsure how the angle is generated though
        // values taken from official maps
        light->aAbB.x = 0.5303301215171814f;
        light->aAbB.y = 0.7071067690849304f;
        light->aAbB.z = 0.5303301215171814f;
        light->aAbB.w = 0.7071067690849304f;

        // light won't light the surface unless canUseShadowMap is set to 1
        // may be to do with lights casting shadows on other lights
        light->canUseShadowMap = 1;
        light->shadowmapVolume = 0; // ignored if this is set to 0

        light->exponent = 0;
        light->priority = 0;
        light->cullDist = 1000;
        light->mipDistance = 0.0f;
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
                const vec4_t sunLightColor = BSPEditableConstants::SUNLIGHT_COLOR;
                const vec3_t sunLightDirection = BSPEditableConstants::SUNLIGHT_DIRECTION;
                light->type = GFX_LIGHT_TYPE_DIR;
                light->diffuseColor.r = sunLightColor.r;
                light->diffuseColor.g = sunLightColor.g;
                light->diffuseColor.a = sunLightColor.a;
                light->dir.x = sunLightDirection.x;
                light->dir.y = sunLightDirection.y;
                light->dir.z = sunLightDirection.z;
            }
            else
            {
                BSPLight* bspLight = &bsp->lights.at(lightIdx - BSPGameConstants::BSP_DEFAULT_LIGHT_COUNT);
                if (bspLight->type == LIGHT_TYPE_DIRECTIONAL)
                {
                    light->type = GFX_LIGHT_TYPE_DIR;
                    light->defName = "white_light";
                    light->roundness = 1.0f;
                    light->cosHalfFovInner = cosf(bspLight->innerConeAngle);
                    light->cosHalfFovOuter = cosf(bspLight->outerConeAngle);
                    light->cosHalfFovExpanded = cosf(bspLight->outerConeAngle);
                }
                else if (bspLight->type == LIGHT_TYPE_SPOT)
                {
                    light->type = GFX_LIGHT_TYPE_SPOT;
                    light->defName = "white_light";
                    light->roundness = 1.0f;
                    light->cosHalfFovInner = cosf(bspLight->innerConeAngle);
                    light->cosHalfFovOuter = cosf(bspLight->outerConeAngle);
                    light->cosHalfFovExpanded = cosf(bspLight->outerConeAngle);
                }
                else // LIGHT_TYPE_POINT
                {
                    light->type = GFX_LIGHT_TYPE_OMNI;
                    light->defName = "white_light_cube";
                    light->roundness = 0.0f;
                    light->cosHalfFovInner = cosf(30.0f * (std::numbers::pi_v<float> / 180.0f));
                    light->cosHalfFovOuter = cosf(55.0f * (std::numbers::pi_v<float> / 180.0f));
                    light->cosHalfFovExpanded = cosf(55.0f * (std::numbers::pi_v<float> / 180.0f));
                }
                setLightCommonValues(light, bspLight);
            }
        }

        return comWorld;
    }
} // namespace BSP
