#include "JsonWeaponCamoLoader.h"

#include "Game/T6/CommonT6.h"
#include "Game/T6/Json/JsonWeaponCamo.h"

#include <format>
#include <iostream>
#include <nlohmann/json.hpp>

using namespace nlohmann;
using namespace T6;

namespace
{
    class JsonLoader
    {
    public:
        JsonLoader(std::istream& stream, MemoryManager& memory, IAssetLoadingManager& manager, std::vector<XAssetInfoGeneric*>& dependencies)
            : m_stream(stream),
              m_memory(memory),
              m_manager(manager),
              m_dependencies(dependencies)

        {
        }

        bool Load(WeaponCamo& weaponCamo) const
        {
            const auto jRoot = json::parse(m_stream);
            std::string type;
            unsigned version;

            jRoot.at("_type").get_to(type);
            jRoot.at("_version").get_to(version);

            if (type != "weaponCamo" || version != 1u)
            {
                std::cerr << "Tried to load weapon camo \"" << weaponCamo.name << "\" but did not find expected type weaponCamo of version 1\n";
                return false;
            }

            const auto jWeaponCamo = jRoot.get<JsonWeaponCamo>();
            return CreateWeaponCamoFromJson(jWeaponCamo, weaponCamo);
        }

    private:
        static void PrintError(const WeaponCamo& weaponCamo, const std::string& message)
        {
            std::cerr << "Cannot load weapon camo \"" << weaponCamo.name << "\": " << message << "\n";
        }

        bool CreateWeaponCamoSetFromJson(const JsonWeaponCamoSet& jWeaponCamoSet, WeaponCamoSet& weaponCamoSet, const WeaponCamo& weaponCamo) const
        {
            if (jWeaponCamoSet.solidCamoImage)
            {
                auto* image = static_cast<XAssetInfo<GfxImage>*>(m_manager.LoadDependency(ASSET_TYPE_IMAGE, jWeaponCamoSet.solidCamoImage.value()));
                if (!image)
                {
                    PrintError(weaponCamo, "Could not find solidCamoImage");
                    return false;
                }
                m_dependencies.push_back(image);
                weaponCamoSet.solidCamoImage = image->Asset();
            }

            if (jWeaponCamoSet.patternCamoImage)
            {
                auto* image = static_cast<XAssetInfo<GfxImage>*>(m_manager.LoadDependency(ASSET_TYPE_IMAGE, jWeaponCamoSet.patternCamoImage.value()));
                if (!image)
                {
                    PrintError(weaponCamo, "Could not find patternCamoImage");
                    return false;
                }
                m_dependencies.push_back(image);
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
            weaponCamoMaterial.replaceFlags = static_cast<uint16_t>(jWeaponCamoMaterial.replaceFlags);

            if (jWeaponCamoMaterial.baseMaterials.size() != jWeaponCamoMaterial.camoMaterials.size())
            {
                PrintError(weaponCamo, "baseMaterials and camoMaterials arrays must have the same amount of entries");
                return false;
            }

            weaponCamoMaterial.numBaseMaterials = static_cast<uint16_t>(jWeaponCamoMaterial.baseMaterials.size());
            if (weaponCamoMaterial.numBaseMaterials > 0)
            {
                weaponCamoMaterial.baseMaterials = static_cast<Material**>(m_memory.Alloc(sizeof(Material*) * weaponCamoMaterial.numBaseMaterials));
                weaponCamoMaterial.camoMaterials = static_cast<Material**>(m_memory.Alloc(sizeof(Material*) * weaponCamoMaterial.numBaseMaterials));
                memset(weaponCamoMaterial.baseMaterials, 0, sizeof(Material*) * weaponCamoMaterial.numBaseMaterials);
                memset(weaponCamoMaterial.camoMaterials, 0, sizeof(Material*) * weaponCamoMaterial.numBaseMaterials);

                for (auto i = 0u; i < weaponCamoMaterial.numBaseMaterials; i++)
                {
                    auto* baseMaterial =
                        static_cast<XAssetInfo<Material>*>(m_manager.LoadDependency(ASSET_TYPE_MATERIAL, jWeaponCamoMaterial.baseMaterials[i]));
                    auto* camoMaterial =
                        static_cast<XAssetInfo<Material>*>(m_manager.LoadDependency(ASSET_TYPE_MATERIAL, jWeaponCamoMaterial.camoMaterials[i]));

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

                    m_dependencies.push_back(baseMaterial);
                    m_dependencies.push_back(camoMaterial);

                    weaponCamoMaterial.baseMaterials[i] = baseMaterial->Asset();
                    weaponCamoMaterial.camoMaterials[i] = camoMaterial->Asset();
                }
            }
            else
            {
                weaponCamoMaterial.baseMaterials = nullptr;
                weaponCamoMaterial.camoMaterials = nullptr;
            }

            return true;
        }

        bool CreateWeaponCamoMaterialSetFromJson(const JsonWeaponCamoMaterialSet& jWeaponCamoMaterialSet,
                                                 WeaponCamoMaterialSet& weaponCamoMaterialSet,
                                                 const WeaponCamo& weaponCamo) const
        {
            if (!jWeaponCamoMaterialSet.materials.empty())
            {
                weaponCamoMaterialSet.numMaterials = jWeaponCamoMaterialSet.materials.size();
                weaponCamoMaterialSet.materials =
                    static_cast<WeaponCamoMaterial*>(m_memory.Alloc(sizeof(WeaponCamoMaterial) * weaponCamoMaterialSet.numMaterials));
                memset(weaponCamoMaterialSet.materials, 0, sizeof(WeaponCamoMaterial) * weaponCamoMaterialSet.numMaterials);

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
                auto* image = static_cast<XAssetInfo<GfxImage>*>(m_manager.LoadDependency(ASSET_TYPE_IMAGE, jWeaponCamo.solidBaseImage.value()));
                if (!image)
                {
                    PrintError(weaponCamo, "Could not find solidBaseImage");
                    return false;
                }
                m_dependencies.push_back(image);
                weaponCamo.solidBaseImage = image->Asset();
            }

            if (jWeaponCamo.patternBaseImage)
            {
                auto* image = static_cast<XAssetInfo<GfxImage>*>(m_manager.LoadDependency(ASSET_TYPE_IMAGE, jWeaponCamo.patternBaseImage.value()));
                if (!image)
                {
                    PrintError(weaponCamo, "Could not find patternBaseImage");
                    return false;
                }
                m_dependencies.push_back(image);
                weaponCamo.patternBaseImage = image->Asset();
            }

            if (!jWeaponCamo.camoSets.empty())
            {
                weaponCamo.numCamoSets = jWeaponCamo.camoSets.size();
                weaponCamo.camoSets = static_cast<WeaponCamoSet*>(m_memory.Alloc(sizeof(WeaponCamoSet) * weaponCamo.numCamoSets));
                memset(weaponCamo.camoSets, 0, sizeof(WeaponCamoSet) * weaponCamo.numCamoSets);

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
                weaponCamo.numCamoMaterials = jWeaponCamo.camoMaterials.size();
                weaponCamo.camoMaterials = static_cast<WeaponCamoMaterialSet*>(m_memory.Alloc(sizeof(WeaponCamoMaterialSet) * weaponCamo.numCamoMaterials));
                memset(weaponCamo.camoMaterials, 0, sizeof(WeaponCamoMaterialSet) * weaponCamo.numCamoMaterials);

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
        IAssetLoadingManager& m_manager;
        std::vector<XAssetInfoGeneric*>& m_dependencies;
    };
} // namespace

namespace T6
{
    bool LoadWeaponCamoAsJson(
        std::istream& stream, WeaponCamo& weaponCamo, MemoryManager* memory, IAssetLoadingManager* manager, std::vector<XAssetInfoGeneric*>& dependencies)
    {
        const JsonLoader loader(stream, *memory, *manager, dependencies);

        return loader.Load(weaponCamo);
    }
} // namespace T6
