#include "CamoJsonLoaderT6.h"

#include "Game/T6/Json/JsonWeaponCamo.h"
#include "Game/T6/T6.h"
#include "Pool/GlobalAssetPool.h"
#include "Weapon/CamoCommon.h"

#include <cstring>
#include <format>
#include <iostream>
#include <nlohmann/json.hpp>

using namespace nlohmann;
using namespace T6;
using namespace ::camo;

namespace
{
    class JsonLoader
    {
    public:
        JsonLoader(std::istream& stream, MemoryManager& memory, AssetCreationContext& context, AssetRegistration<AssetWeaponCamo>& registration)
            : m_stream(stream),
              m_memory(memory),
              m_context(context),
              m_registration(registration)

        {
        }

        bool Load(WeaponCamo& weaponCamo) const
        {
            try
            {
                const auto jRoot = json::parse(m_stream);
                std::string type;
                unsigned version;

                jRoot.at("_type").get_to(type);
                jRoot.at("_version").get_to(version);

                if (type != "weaponCamo" || version != 1u)
                {
                    std::cerr << std::format("Tried to load weapon camo \"{}\" but did not find expected type weaponCamo of version {}\n", weaponCamo.name, 1u);
                    return false;
                }

                const auto jWeaponCamo = jRoot.get<JsonWeaponCamo>();
                return CreateWeaponCamoFromJson(jWeaponCamo, weaponCamo);
            }
            catch (const json::exception& e)
            {
                std::cerr << std::format("Failed to parse json of weapon camo: {}\n", e.what());
            }

            return false;
        }

    private:
        static void PrintError(const WeaponCamo& weaponCamo, const std::string& message)
        {
            std::cerr << std::format("Cannot load weapon camo \"{}\": {}\n", weaponCamo.name, message);
        }

        bool CreateWeaponCamoSetFromJson(const JsonWeaponCamoSet& jWeaponCamoSet, WeaponCamoSet& weaponCamoSet, const WeaponCamo& weaponCamo) const
        {
            if (jWeaponCamoSet.solidCamoImage)
            {
                auto* image = m_context.LoadDependency<AssetImage>(jWeaponCamoSet.solidCamoImage.value());
                if (!image)
                {
                    PrintError(weaponCamo, "Could not find solidCamoImage");
                    return false;
                }
                m_registration.AddDependency(image);
                weaponCamoSet.solidCamoImage = image->Asset();
            }

            if (jWeaponCamoSet.patternCamoImage)
            {
                auto* image = m_context.LoadDependency<AssetImage>(jWeaponCamoSet.patternCamoImage.value());
                if (!image)
                {
                    PrintError(weaponCamo, "Could not find patternCamoImage");
                    return false;
                }
                m_registration.AddDependency(image);
                weaponCamoSet.patternCamoImage = image->Asset();
            }

            weaponCamoSet.patternOffset.x = jWeaponCamoSet.patternOffset.x;
            weaponCamoSet.patternOffset.y = jWeaponCamoSet.patternOffset.y;
            weaponCamoSet.patternScale = jWeaponCamoSet.patternScale;

            return true;
        }

        bool CreateWeaponCamoMaterialFromJson(const JsonWeaponCamoMaterial& jWeaponCamoMaterial,
                                              WeaponCamoMaterial& weaponCamoMaterial,
                                              const WeaponCamo& weaponCamo) const
        {
            if (jWeaponCamoMaterial.useColorMap)
                weaponCamoMaterial.replaceFlags |= WCM_REPLACE_COLOR;
            if (jWeaponCamoMaterial.useNormalMap)
                weaponCamoMaterial.replaceFlags |= WCM_REPLACE_NORMAL;
            if (jWeaponCamoMaterial.useSpecularMap)
                weaponCamoMaterial.replaceFlags |= WCM_REPLACE_SPECULAR;

            weaponCamoMaterial.numBaseMaterials = static_cast<uint16_t>(jWeaponCamoMaterial.materialOverrides.size());
            if (!jWeaponCamoMaterial.materialOverrides.empty())
            {
                weaponCamoMaterial.baseMaterials = m_memory.Alloc<Material*>(weaponCamoMaterial.numBaseMaterials);
                weaponCamoMaterial.camoMaterials = m_memory.Alloc<Material*>(weaponCamoMaterial.numBaseMaterials);

                for (auto i = 0u; i < weaponCamoMaterial.numBaseMaterials; i++)
                {
                    const auto& materialOverride = jWeaponCamoMaterial.materialOverrides[i];
                    auto* baseMaterial = m_context.LoadDependency<AssetMaterial>(materialOverride.baseMaterial);
                    auto* camoMaterial = m_context.LoadDependency<AssetMaterial>(materialOverride.camoMaterial);

                    if (!baseMaterial)
                    {
                        PrintError(weaponCamo, "Could not find baseMaterial");
                        return false;
                    }

                    if (!camoMaterial)
                    {
                        PrintError(weaponCamo, "Could not find camoMaterial");
                        return false;
                    }

                    m_registration.AddDependency(baseMaterial);
                    m_registration.AddDependency(camoMaterial);

                    weaponCamoMaterial.baseMaterials[i] = baseMaterial->Asset();
                    weaponCamoMaterial.camoMaterials[i] = camoMaterial->Asset();
                }
            }
            else
            {
                weaponCamoMaterial.baseMaterials = nullptr;
                weaponCamoMaterial.camoMaterials = nullptr;
            }

            for (auto i = 0u; i < std::extent_v<decltype(WeaponCamoMaterial::shaderConsts)>; i++)
                weaponCamoMaterial.shaderConsts[i] = jWeaponCamoMaterial.shaderConsts[i];

            return true;
        }

        bool CreateWeaponCamoMaterialSetFromJson(const JsonWeaponCamoMaterialSet& jWeaponCamoMaterialSet,
                                                 WeaponCamoMaterialSet& weaponCamoMaterialSet,
                                                 const WeaponCamo& weaponCamo) const
        {
            if (!jWeaponCamoMaterialSet.materials.empty())
            {
                weaponCamoMaterialSet.numMaterials = static_cast<unsigned>(jWeaponCamoMaterialSet.materials.size());
                weaponCamoMaterialSet.materials = m_memory.Alloc<WeaponCamoMaterial>(weaponCamoMaterialSet.numMaterials);

                for (auto i = 0u; i < weaponCamoMaterialSet.numMaterials; i++)
                {
                    if (!CreateWeaponCamoMaterialFromJson(jWeaponCamoMaterialSet.materials[i], weaponCamoMaterialSet.materials[i], weaponCamo))
                        return false;
                }
            }
            else
            {
                weaponCamoMaterialSet.numMaterials = 0;
                weaponCamoMaterialSet.materials = nullptr;
            }

            return true;
        }

        bool CreateWeaponCamoFromJson(const JsonWeaponCamo& jWeaponCamo, WeaponCamo& weaponCamo) const
        {
            if (jWeaponCamo.solidBaseImage)
            {
                auto* image = m_context.LoadDependency<AssetImage>(jWeaponCamo.solidBaseImage.value());
                if (!image)
                {
                    PrintError(weaponCamo, "Could not find solidBaseImage");
                    return false;
                }
                m_registration.AddDependency(image);
                weaponCamo.solidBaseImage = image->Asset();
            }

            if (jWeaponCamo.patternBaseImage)
            {
                auto* image = m_context.LoadDependency<AssetImage>(jWeaponCamo.patternBaseImage.value());
                if (!image)
                {
                    PrintError(weaponCamo, "Could not find patternBaseImage");
                    return false;
                }
                m_registration.AddDependency(image);
                weaponCamo.patternBaseImage = image->Asset();
            }

            if (!jWeaponCamo.camoSets.empty())
            {
                weaponCamo.numCamoSets = static_cast<unsigned>(jWeaponCamo.camoSets.size());
                weaponCamo.camoSets = m_memory.Alloc<WeaponCamoSet>(weaponCamo.numCamoSets);

                for (auto i = 0u; i < weaponCamo.numCamoSets; i++)
                {
                    if (!CreateWeaponCamoSetFromJson(jWeaponCamo.camoSets[i], weaponCamo.camoSets[i], weaponCamo))
                        return false;
                }
            }
            else
            {
                weaponCamo.numCamoSets = 0;
                weaponCamo.camoSets = nullptr;
            }

            if (!jWeaponCamo.camoMaterials.empty())
            {
                weaponCamo.numCamoMaterials = static_cast<unsigned>(jWeaponCamo.camoMaterials.size());
                weaponCamo.camoMaterials = m_memory.Alloc<WeaponCamoMaterialSet>(weaponCamo.numCamoMaterials);

                for (auto i = 0u; i < weaponCamo.numCamoMaterials; i++)
                {
                    if (!CreateWeaponCamoMaterialSetFromJson(jWeaponCamo.camoMaterials[i], weaponCamo.camoMaterials[i], weaponCamo))
                        return false;
                }
            }
            else
            {
                weaponCamo.numCamoMaterials = 0;
                weaponCamo.camoMaterials = nullptr;
            }

            return true;
        }

        std::istream& m_stream;
        MemoryManager& m_memory;
        AssetCreationContext& m_context;
        AssetRegistration<AssetWeaponCamo>& m_registration;
    };

    class WeaponCamoLoader final : public AssetCreator<AssetWeaponCamo>
    {
    public:
        WeaponCamoLoader(MemoryManager& memory, ISearchPath& searchPath)
            : m_memory(memory),
              m_search_path(searchPath)
        {
        }

        AssetCreationResult CreateAsset(const std::string& assetName, AssetCreationContext& context) override
        {
            const auto file = m_search_path.Open(GetJsonFileNameForAssetName(assetName));
            if (!file.IsOpen())
                return AssetCreationResult::NoAction();

            auto* weaponCamo = m_memory.Alloc<WeaponCamo>();
            weaponCamo->name = m_memory.Dup(assetName.c_str());

            AssetRegistration<AssetWeaponCamo> registration(assetName, weaponCamo);
            const JsonLoader loader(*file.m_stream, m_memory, context, registration);
            if (!loader.Load(*weaponCamo))
            {
                std::cerr << std::format("Failed to load weapon camo \"{}\"\n", assetName);
                return AssetCreationResult::Failure();
            }

            return AssetCreationResult::Success(context.AddAsset(std::move(registration)));
        }

    private:
        MemoryManager& m_memory;
        ISearchPath& m_search_path;
    };
} // namespace

namespace T6::camo
{
    std::unique_ptr<AssetCreator<AssetWeaponCamo>> CreateJsonLoader(MemoryManager& memory, ISearchPath& searchPath)
    {
        return std::make_unique<WeaponCamoLoader>(memory, searchPath);
    }
} // namespace T6::camo
