#include "AssetLoaderMaterial.h"

#include <cstring>
#include <iostream>
#include <sstream>

#include "ObjLoading.h"
#include "AssetLoading/AbstractGdtEntryReader.h"
#include "Game/IW4/CommonIW4.h"
#include "Game/IW4/IW4.h"
#include "Game/IW4/ObjConstantsIW4.h"
#include "Pool/GlobalAssetPool.h"

using namespace IW4;

namespace IW4
{
    class MaterialGdtLoader : AbstractGdtEntryReader
    {
    public:
        MaterialGdtLoader(const GdtEntry& entry, MemoryManager* memory, IAssetLoadingManager* manager)
            : AbstractGdtEntryReader(entry),
              m_memory(memory),
              m_manager(manager),
              m_material(nullptr)
        {
        }

        bool Load()
        {
            m_material = m_memory->Create<Material>();
            memset(m_material, 0, sizeof(Material));

            m_material->info.name = m_memory->Dup(m_entry.m_name.c_str());
            material_template();

            FinalizeMaterial();
            return true;
        }

        _NODISCARD Material* GetMaterial() const
        {
            return m_material;
        }

        _NODISCARD std::vector<XAssetInfoGeneric*> GetDependencies()
        {
            return std::move(m_dependencies);
        }

    private:
        void material_template()
        {
            const auto materialType = ReadStringProperty("materialType");

            if (materialType == "model phong" || materialType == "world phong" || materialType == "impact mark")
            {
                mtl_phong_template();
            }
            else if (materialType == "model ambient")
            {
                mtl_ambient_template();
            }
            else if (materialType == "2d")
            {
                mtl_2d_template();
            }
            else if (materialType == "model unlit" || materialType == "world unlit")
            {
                mtl_unlit_template();
            }
            else if (materialType == "unlit")
            {
                mtl_unlit_deprecated_template();
            }
            else if (materialType == "effect")
            {
                mtl_effect_template();
            }
            else if (materialType == "distortion")
            {
                mtl_distortion_template();
            }
            else if (materialType == "particle cloud")
            {
                mtl_particlecloud_template();
            }
            else if (materialType == "tools")
            {
                mtl_tools_template();
            }
            else if (materialType == "sky")
            {
                mtl_sky_template();
            }
            else if (materialType == "water")
            {
                mtl_water_template();
            }
            else if (materialType == "objective")
            {
                mtl_objective_template();
            }
            else if (materialType == "custom")
            {
                custom_template();
            }
            else
            {
                std::ostringstream ss;
                ss << "Unknown material type: \"" << materialType << "\"";
                throw GdtReadingException(ss.str());
            }
        }

        void mtl_phong_template()
        {
        }

        void mtl_ambient_template()
        {
        }

        void mtl_2d_template()
        {
            commonsetup_template();

            SetTechniqueSet("2d");

            const auto colorMapName = ReadStringProperty("colorMap");

            if (!colorMapName.empty())
                AddMapTexture("colorMap", TS_2D, colorMapName);
            else
                throw GdtReadingException("ColorMap may not be blank in 2d materials");
        }

        void mtl_unlit_template()
        {
        }

        void mtl_unlit_deprecated_template()
        {
        }

        void mtl_effect_template()
        {
        }

        void mtl_distortion_template()
        {
        }

        void mtl_particlecloud_template()
        {
        }

        void mtl_tools_template()
        {
        }

        void mtl_sky_template()
        {
        }

        void mtl_water_template()
        {
        }

        void mtl_objective_template()
        {
        }

        void custom_template()
        {
            const auto customTemplate = ReadStringProperty("customTemplate");

            if (customTemplate == "mtl_custom")
            {
                mtl_custom_template();
            }
            else if (customTemplate == "mtl_phong_flag")
            {
                mtl_phong_flag_template();
            }
            else if (customTemplate == "mtl_grain_overlay")
            {
                mtl_grain_overlay_template();
            }
            else if (customTemplate == "mtl_effect_eyeoffset")
            {
                mtl_effect_eyeoffset_template();
            }
            else if (customTemplate == "mtl_reflexsight")
            {
                mtl_reflexsight_template();
            }
            else if (customTemplate == "mtl_shadowclear")
            {
                mtl_shadowclear_template();
            }
            else if (customTemplate == "mtl_shadowoverlay")
            {
                mtl_shadowoverlay_template();
            }
            else if (customTemplate == "mtl_splatter")
            {
                mtl_splatter_template();
            }
            else
            {
                std::ostringstream ss;
                ss << "Unknown custom template: \"" << customTemplate << "\"";
                throw GdtReadingException(ss.str());
            }
        }

        void mtl_custom_template()
        {
        }

        void mtl_phong_flag_template()
        {
        }

        void mtl_grain_overlay_template()
        {
        }

        void mtl_effect_eyeoffset_template()
        {
        }

        void mtl_reflexsight_template()
        {
        }

        void mtl_shadowclear_template()
        {
        }

        void mtl_shadowoverlay_template()
        {
        }

        void mtl_splatter_template()
        {
        }

        void commonsetup_template()
        {
        }

        void SetTechniqueSet(const std::string& techsetName)
        {
            auto* techset = reinterpret_cast<XAssetInfo<MaterialTechniqueSet>*>(m_manager->LoadDependency(ASSET_TYPE_TECHNIQUE_SET, techsetName));

            if (techset == nullptr)
            {
                std::ostringstream ss;
                ss << "Could not load techset: \"" << techsetName << "\"";
                throw GdtReadingException(ss.str());
            }

            m_dependencies.push_back(techset);
            m_material->techniqueSet = techset->Asset();
        }

        void AddMapTexture(const std::string& typeName, const TextureSemantic semantic, const std::string& textureName)
        {
            MaterialTextureDef textureDef{};
            textureDef.nameHash = Common::R_HashString(typeName.c_str());
            textureDef.nameStart = typeName[0];
            textureDef.nameEnd = typeName[typeName.size() - 1];
            textureDef.samplerState = 0; // TODO
            textureDef.semantic = static_cast<unsigned char>(semantic);

            auto* image = reinterpret_cast<XAssetInfo<GfxImage>*>(m_manager->LoadDependency(ASSET_TYPE_IMAGE, textureName));

            if (image == nullptr)
            {
                std::ostringstream ss;
                ss << "Could not load image: \"" << textureName << "\"";
                throw GdtReadingException(ss.str());
            }

            m_dependencies.push_back(image);
            textureDef.u.image = image->Asset();

            m_textures.push_back(textureDef);
        }

        void FinalizeMaterial() const
        {
            if (!m_textures.empty())
            {
                m_material->textureTable = static_cast<MaterialTextureDef*>(m_memory->Alloc(sizeof(MaterialTextureDef) * m_textures.size()));
                m_material->textureCount = m_textures.size();
                memcpy(m_material->textureTable, m_textures.data(), sizeof(MaterialTextureDef) * m_textures.size());
            }
            else
            {
                m_material->textureTable = nullptr;
                m_material->textureCount = 0u;
            }
        }

        MemoryManager* m_memory;
        IAssetLoadingManager* m_manager;
        std::vector<XAssetInfoGeneric*> m_dependencies;

        Material* m_material;
        std::vector<MaterialTextureDef> m_textures;
    };
}

void* AssetLoaderMaterial::CreateEmptyAsset(const std::string& assetName, MemoryManager* memory)
{
    auto* material = memory->Create<Material>();
    memset(material, 0, sizeof(Material));
    material->info.name = memory->Dup(assetName.c_str());
    return material;
}

bool AssetLoaderMaterial::CanLoadFromGdt() const
{
    return true;
}

bool AssetLoaderMaterial::LoadFromGdt(const std::string& assetName, IGdtQueryable* gdtQueryable, MemoryManager* memory, IAssetLoadingManager* manager, Zone* zone) const
{
    const auto* entry = gdtQueryable->GetGdtEntryByGdfAndName(ObjConstants::GDF_FILENAME_MATERIAL, assetName);
    if (!entry)
        return false;

    MaterialGdtLoader loader(*entry, memory, manager);

    try
    {
        if (loader.Load())
            manager->AddAsset(ASSET_TYPE_MATERIAL, assetName, loader.GetMaterial(), loader.GetDependencies(), std::vector<scr_string_t>());
    }
    catch (const GdtReadingException& e)
    {
        std::cerr << "Error while trying to load material from gdt: " << e.what() << " @ GdtEntry \"" << entry->m_name << "\"\n";
        return false;
    }

    return true;
}
