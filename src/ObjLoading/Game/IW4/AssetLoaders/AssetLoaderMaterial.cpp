#include "AssetLoaderMaterial.h"

#include <cstring>
#include <iostream>
#include <sstream>

#include "ObjLoading.h"
#include "AssetLoading/AbstractGdtEntryReader.h"
#include "Game/IW4/CommonIW4.h"
#include "Game/IW4/IW4.h"
#include "Game/IW4/MaterialConstantsIW4.h"
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
              m_material(nullptr),
              m_base_statebits{}
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

            if (materialType == GDT_MATERIAL_TYPE_MODEL_PHONG
                || materialType == GDT_MATERIAL_TYPE_WORLD_PHONG
                || materialType == GDT_MATERIAL_TYPE_IMPACT_MARK)
            {
                mtl_phong_template();
            }
            else if (materialType == GDT_MATERIAL_TYPE_MODEL_AMBIENT)
            {
                mtl_ambient_template();
            }
            else if (materialType == GDT_MATERIAL_TYPE_2D)
            {
                mtl_2d_template();
            }
            else if (materialType == GDT_MATERIAL_TYPE_MODEL_UNLIT
                || materialType == GDT_MATERIAL_TYPE_WORLD_UNLIT)
            {
                mtl_unlit_template();
            }
            else if (materialType == GDT_MATERIAL_TYPE_UNLIT)
            {
                mtl_unlit_deprecated_template();
            }
            else if (materialType == GDT_MATERIAL_TYPE_EFFECT)
            {
                mtl_effect_template();
            }
            else if (materialType == GDT_MATERIAL_TYPE_DISTORTION)
            {
                mtl_distortion_template();
            }
            else if (materialType == GDT_MATERIAL_TYPE_PARTICLE_CLOUD)
            {
                mtl_particlecloud_template();
            }
            else if (materialType == GDT_MATERIAL_TYPE_TOOLS)
            {
                mtl_tools_template();
            }
            else if (materialType == GDT_MATERIAL_TYPE_SKY)
            {
                mtl_sky_template();
            }
            else if (materialType == GDT_MATERIAL_TYPE_WATER)
            {
                mtl_water_template();
            }
            else if (materialType == GDT_MATERIAL_TYPE_OBJECTIVE)
            {
                mtl_objective_template();
            }
            else if (materialType == GDT_MATERIAL_TYPE_CUSTOM)
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

            if (customTemplate == GDT_CUSTOM_MATERIAL_TYPE_CUSTOM)
            {
                mtl_custom_template();
            }
            else if (customTemplate == GDT_CUSTOM_MATERIAL_TYPE_PHONG_FLAG)
            {
                mtl_phong_flag_template();
            }
            else if (customTemplate == GDT_CUSTOM_MATERIAL_TYPE_GRAIN_OVERLAY)
            {
                mtl_grain_overlay_template();
            }
            else if (customTemplate == GDT_CUSTOM_MATERIAL_TYPE_EFFECT_EYE_OFFSET)
            {
                mtl_effect_eyeoffset_template();
            }
            else if (customTemplate == GDT_CUSTOM_MATERIAL_TYPE_REFLEXSIGHT)
            {
                mtl_reflexsight_template();
            }
            else if (customTemplate == GDT_CUSTOM_MATERIAL_TYPE_SHADOWCLEAR)
            {
                mtl_shadowclear_template();
            }
            else if (customTemplate == GDT_CUSTOM_MATERIAL_TYPE_SHADOWOVERLAY)
            {
                mtl_shadowoverlay_template();
            }
            else if (customTemplate == GDT_CUSTOM_MATERIAL_TYPE_SPLATTER)
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
            refblend_template();
            sort_template();
            clamp_template();

            // tessSize

            textureAtlas_template();

            // hasEditorMaterial

            // allocLightmap

            statebits_template();
        }

        void refblend_template()
        {
            const auto blendFunc = ReadStringProperty("blendFunc");
        }

        void sort_template()
        {
            const auto sort = ReadStringProperty("sort");
            const auto materialType = ReadStringProperty("materialType");
            const auto polygonOffset = ReadStringProperty("polygonOffset");
            const auto blendFunc = ReadStringProperty("blendFunc");

            std::string sortKey;
            if (sort.empty() || sort == GDT_SORTKEY_DEFAULT)
            {
                if (materialType == GDT_MATERIAL_TYPE_DISTORTION)
                    sortKey = GDT_SORTKEY_DISTORTION;
                else if (polygonOffset == "Static Decal")
                    sortKey = GDT_SORTKEY_DECAL_STATIC;
                else if (polygonOffset == "Weapon Impact")
                    sortKey = GDT_SORTKEY_DECAL_WEAPON_IMPACT;
                else if (materialType == GDT_MATERIAL_TYPE_EFFECT)
                    sortKey = GDT_SORTKEY_EFFECT_AUTO_SORT;
                else if (materialType == GDT_MATERIAL_TYPE_OBJECTIVE
                    || blendFunc == "Blend" || blendFunc == "Add" || blendFunc == "Screen Add")
                    sortKey = GDT_SORTKEY_BLEND_ADDITIVE;
                    // else if (blendFunc == "Multiply") // TODO
                    //     sortKey = GDT_SORTKEY_MULTIPLICATIVE;
                else if (materialType == GDT_MATERIAL_TYPE_SKY)
                    sortKey = GDT_SORTKEY_SKY;
                else if (materialType == GDT_MATERIAL_TYPE_MODEL_AMBIENT)
                    sortKey = GDT_SORTKEY_OPAQUE_AMBIENT;
                else
                    sortKey = GDT_SORTKEY_OPAQUE;
            }
            else
                sortKey = sort;

            bool foundSortKey = false;
            for (auto sortKeyIndex = 0u; sortKeyIndex < SORTKEY_MAX; sortKeyIndex++)
            {
                if (sortKey == SortKeyNames[sortKeyIndex])
                {
                    SetSort(static_cast<unsigned char>(sortKeyIndex));
                    foundSortKey = true;
                    break;
                }
            }

            if (!foundSortKey)
            {
                char* endPtr;
                const auto sortKeyNum = strtoul(sortKey.c_str(), &endPtr, 10);

                if (endPtr != &sortKey[sortKey.size()])
                {
                    std::ostringstream ss;
                    ss << "Invalid sort value: \"" << sortKey << "\"";
                    throw GdtReadingException(ss.str());
                }

                SetSort(static_cast<unsigned char>(sortKeyNum));
            }
        }

        void clamp_template()
        {
        }

        void textureAtlas_template()
        {
            const auto rowCount = ReadIntegerProperty("textureAtlasRowCount", 1);
            const auto columnCount = ReadIntegerProperty("textureAtlasColumnCount", 1);

            SetTextureAtlas(static_cast<unsigned char>(rowCount), static_cast<unsigned char>(columnCount));
        }

        void statebits_template()
        {
            alphatest_template();
            blendfunc_template();
            colorwrite_template();
            cullface_template();
            depthtest_template();
            depthwrite_template();
            polygonoffset_template();
            stencil_template();
        }

        void alphatest_template()
        {
            const auto alphaTest = ReadStringProperty("alphaTest");

            if (alphaTest == GDT_ALPHA_TEST_ALWAYS)
                SetAlphaTest(AlphaTest_e::ALWAYS);
            else if (alphaTest == GDT_ALPHA_TEST_GE128)
                SetAlphaTest(AlphaTest_e::GE128);
            else
            {
                std::ostringstream ss;
                ss << "Invalid alphatest value: \"" << alphaTest << "\"";
                throw GdtReadingException(ss.str());
            }
        }

        void blendfunc_template()
        {
            const auto blendFunc = ReadStringProperty("blendFunc");

            if (blendFunc == GDT_BLEND_FUNC_ADD)
            {
                SetBlendFunc(BlendOp_e::ADD, CustomBlendFunc_e::ONE, CustomBlendFunc_e::ONE);
                SetSeparateAlphaBlendFunc(BlendOp_e::DISABLE, CustomBlendFunc_e::ONE, CustomBlendFunc_e::ZERO);
            }
            else if (blendFunc == GDT_BLEND_FUNC_BLEND)
            {
                SetBlendFunc(BlendOp_e::ADD, CustomBlendFunc_e::SRC_ALPHA, CustomBlendFunc_e::INV_SRC_ALPHA);
                SetSeparateAlphaBlendFunc(BlendOp_e::DISABLE, CustomBlendFunc_e::ONE, CustomBlendFunc_e::ZERO);
            }
            else if (blendFunc == GDT_BLEND_FUNC_MULTIPLY)
            {
                SetBlendFunc(BlendOp_e::ADD, CustomBlendFunc_e::ZERO, CustomBlendFunc_e::SRC_COLOR);
                SetSeparateAlphaBlendFunc(BlendOp_e::DISABLE, CustomBlendFunc_e::ONE, CustomBlendFunc_e::ZERO);
            }
            else if (blendFunc == GDT_BLEND_FUNC_REPLACE)
            {
                SetBlendFunc(BlendOp_e::DISABLE, CustomBlendFunc_e::ONE, CustomBlendFunc_e::ZERO);
                SetSeparateAlphaBlendFunc(BlendOp_e::DISABLE, CustomBlendFunc_e::ONE, CustomBlendFunc_e::ZERO);
            }
            else if (blendFunc == GDT_BLEND_FUNC_SCREEN_ADD)
            {
                SetBlendFunc(BlendOp_e::ADD, CustomBlendFunc_e::INV_DST_ALPHA, CustomBlendFunc_e::ONE);
                SetSeparateAlphaBlendFunc(BlendOp_e::DISABLE, CustomBlendFunc_e::ONE, CustomBlendFunc_e::ZERO);
            }
            else if (blendFunc == GDT_BLEND_FUNC_CUSTOM)
            {
                const auto customBlendOpRgb = ReadEnumProperty<BlendOp_e>("customBlendOpRgb", GdtBlendOpNames, std::extent_v<decltype(GdtBlendOpNames)>);
                const auto srcCustomBlendFunc = ReadEnumProperty<CustomBlendFunc_e>("srcCustomBlendFunc", GdtCustomBlendFuncNames, std::extent_v<decltype(GdtCustomBlendFuncNames)>);
                const auto destCustomBlendFunc = ReadEnumProperty<CustomBlendFunc_e>("destCustomBlendFunc", GdtCustomBlendFuncNames, std::extent_v<decltype(GdtCustomBlendFuncNames)>);
                const auto customBlendOpAlpha = ReadEnumProperty<BlendOp_e>("customBlendOpAlpha", GdtBlendOpNames, std::extent_v<decltype(GdtBlendOpNames)>);
                const auto srcCustomBlendFuncAlpha = ReadEnumProperty<CustomBlendFunc_e>("srcCustomBlendFuncAlpha", GdtCustomBlendFuncNames, std::extent_v<decltype(GdtCustomBlendFuncNames)>);
                const auto destCustomBlendFuncAlpha = ReadEnumProperty<CustomBlendFunc_e>("destCustomBlendFuncAlpha", GdtCustomBlendFuncNames, std::extent_v<decltype(GdtCustomBlendFuncNames)>);

                SetBlendFunc(customBlendOpRgb, srcCustomBlendFunc, destCustomBlendFunc);
                SetSeparateAlphaBlendFunc(customBlendOpAlpha, srcCustomBlendFuncAlpha, destCustomBlendFuncAlpha);
            }
            else
            {
                std::ostringstream ss;
                ss << "Invalid blendfunc value: \"" << blendFunc << "\"";
                throw GdtReadingException(ss.str());
            }
        }

        void colorwrite_template()
        {
            const auto colorWriteRed = ReadEnumProperty<StateBitsEnabledStatus_e>("colorWriteRed", GdtStateBitsEnabledStatusNames, std::extent_v<decltype(GdtStateBitsEnabledStatusNames)>);
            const auto colorWriteGreen = ReadEnumProperty<StateBitsEnabledStatus_e>("colorWriteGreen", GdtStateBitsEnabledStatusNames, std::extent_v<decltype(GdtStateBitsEnabledStatusNames)>);
            const auto colorWriteBlue = ReadEnumProperty<StateBitsEnabledStatus_e>("colorWriteBlue", GdtStateBitsEnabledStatusNames, std::extent_v<decltype(GdtStateBitsEnabledStatusNames)>);
            const auto colorWriteAlpha = ReadEnumProperty<StateBitsEnabledStatus_e>("colorWriteAlpha", GdtStateBitsEnabledStatusNames, std::extent_v<decltype(GdtStateBitsEnabledStatusNames)>);

            SetColorWrite(colorWriteRed, colorWriteGreen, colorWriteBlue, colorWriteAlpha);
        }

        void cullface_template()
        {
            const auto cullFace = ReadEnumProperty<CullFace_e>("cullFace", GdtCullFaceNames, std::extent_v<decltype(GdtCullFaceNames)>);

            SetCullFace(cullFace);
        }

        void depthtest_template()
        {
            const auto depthTest = ReadEnumProperty<DepthTest_e>("depthTest", GdtDepthTestNames, std::extent_v<decltype(GdtDepthTestNames)>);

            SetDepthTest(depthTest);
        }

        void depthwrite_template()
        {
            const auto depthWrite = ReadEnumProperty<StateBitsEnabledStatus_e>("depthWrite", GdtStateBitsEnabledStatusNames, std::extent_v<decltype(GdtStateBitsEnabledStatusNames)>);
            const auto blendFunc = ReadStringProperty("blendFunc");

            if (depthWrite == StateBitsEnabledStatus_e::ENABLED)
                SetDepthWrite(true);
            else if (depthWrite == StateBitsEnabledStatus_e::DISABLED)
                SetDepthWrite(false);
            else if (blendFunc == GDT_BLEND_FUNC_ADD)
                SetDepthWrite(false);
            else if (blendFunc == GDT_BLEND_FUNC_BLEND)
                SetDepthWrite(false);
            else if (blendFunc == GDT_BLEND_FUNC_MULTIPLY)
                SetDepthWrite(false);
            else if (blendFunc == GDT_BLEND_FUNC_REPLACE)
                SetDepthWrite(true);
            else if (blendFunc == GDT_BLEND_FUNC_SCREEN_ADD)
                SetDepthWrite(false);
            else if (blendFunc == GDT_BLEND_FUNC_CUSTOM)
                SetDepthWrite(false);
            else
            {
                std::ostringstream ss;
                ss << "Invalid depthWrite blendFunc value: \"" << blendFunc << "\"";
                throw GdtReadingException(ss.str());
            }
        }

        void polygonoffset_template()
        {
            const auto polygonOffset = ReadEnumProperty<PolygonOffset_e>("polygonOffset", GdtPolygonOffsetNames, std::extent_v<decltype(GdtPolygonOffsetNames)>);

            SetPolygonOffset(polygonOffset);
        }

        void stencil_template()
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

        void SetSort(const unsigned char sort) const
        {
            m_material->info.sortKey = sort;
        }

        void SetTextureAtlas(const unsigned char rowCount, const unsigned char columnCount) const
        {
            m_material->info.textureAtlasRowCount = rowCount;
            m_material->info.textureAtlasColumnCount = columnCount;
        }

        void SetAlphaTest(const AlphaTest_e alphaTest)
        {
            switch (alphaTest)
            {
            case AlphaTest_e::ALWAYS:
                m_base_statebits.loadBits[0] |= GFXS0_ATEST_DISABLE;
                break;

            case AlphaTest_e::GT0:
                m_base_statebits.loadBits[0] |= GFXS0_ATEST_GT_0;
                break;

            case AlphaTest_e::LT128:
                m_base_statebits.loadBits[0] |= GFXS0_ATEST_LT_128;
                break;

            case AlphaTest_e::GE128:
                m_base_statebits.loadBits[0] |= GFXS0_ATEST_GE_128;
                break;

            case AlphaTest_e::UNKNOWN:
            default:
                std::ostringstream ss;
                ss << "Unknown alphatest value: \"" << static_cast<int>(alphaTest) << "\"";
                throw GdtReadingException(ss.str());
            }
        }

        void SetBlendFunc(BlendOp_e blendOp, CustomBlendFunc_e srcFunc, CustomBlendFunc_e destFunc)
        {
            if (blendOp == BlendOp_e::UNKNOWN || srcFunc == CustomBlendFunc_e::UNKNOWN || destFunc == CustomBlendFunc_e::UNKNOWN)
            {
                std::ostringstream ss;
                ss << "Unknown SeparateAlphaBlendFunc values: \"\"";
                throw GdtReadingException(ss.str());
            }

            m_base_statebits.loadBits[0] &= ~GFXS0_BLENDOP_RGB_MASK;
            m_base_statebits.loadBits[0] |= ((static_cast<unsigned>(blendOp) - 1) >> GFXS0_BLENDOP_RGB_SHIFT) & GFXS0_BLENDOP_RGB_MASK;

            m_base_statebits.loadBits[0] &= ~GFXS0_SRCBLEND_RGB_MASK;
            m_base_statebits.loadBits[0] |= ((static_cast<unsigned>(srcFunc) - 1) >> GFXS0_SRCBLEND_RGB_SHIFT) & GFXS0_SRCBLEND_RGB_MASK;

            m_base_statebits.loadBits[0] &= ~GFXS0_DSTBLEND_RGB_MASK;
            m_base_statebits.loadBits[0] |= ((static_cast<unsigned>(destFunc) - 1) >> GFXS0_DSTBLEND_RGB_SHIFT) & GFXS0_DSTBLEND_RGB_MASK;
        }

        void SetSeparateAlphaBlendFunc(BlendOp_e blendOp, CustomBlendFunc_e srcFunc, CustomBlendFunc_e destFunc)
        {
            if (blendOp == BlendOp_e::UNKNOWN || srcFunc == CustomBlendFunc_e::UNKNOWN || destFunc == CustomBlendFunc_e::UNKNOWN)
            {
                std::ostringstream ss;
                ss << "Unknown SeparateAlphaBlendFunc values: \"\"";
                throw GdtReadingException(ss.str());
            }

            m_base_statebits.loadBits[0] &= ~GFXS0_BLENDOP_ALPHA_MASK;
            m_base_statebits.loadBits[0] |= ((static_cast<unsigned>(blendOp) - 1) >> GFXS0_BLENDOP_ALPHA_SHIFT) & GFXS0_BLENDOP_ALPHA_MASK;

            m_base_statebits.loadBits[0] &= ~GFXS0_SRCBLEND_ALPHA_MASK;
            m_base_statebits.loadBits[0] |= ((static_cast<unsigned>(srcFunc) - 1) >> GFXS0_SRCBLEND_ALPHA_SHIFT) & GFXS0_SRCBLEND_ALPHA_MASK;

            m_base_statebits.loadBits[0] &= ~GFXS0_DSTBLEND_ALPHA_MASK;
            m_base_statebits.loadBits[0] |= ((static_cast<unsigned>(destFunc) - 1) >> GFXS0_DSTBLEND_ALPHA_SHIFT) & GFXS0_DSTBLEND_ALPHA_MASK;
        }

        void SetColorWrite(const StateBitsEnabledStatus_e colorWriteRed, const StateBitsEnabledStatus_e colorWriteGreen, const StateBitsEnabledStatus_e colorWriteBlue,
                           const StateBitsEnabledStatus_e colorWriteAlpha)
        {
            if (colorWriteRed == StateBitsEnabledStatus_e::UNKNOWN || colorWriteGreen == StateBitsEnabledStatus_e::UNKNOWN
                || colorWriteBlue == StateBitsEnabledStatus_e::UNKNOWN || colorWriteAlpha == StateBitsEnabledStatus_e::UNKNOWN)
            {
                std::ostringstream ss;
                ss << "Unknown ColorWrite values: \"\"";
                throw GdtReadingException(ss.str());
            }

            if (colorWriteRed != colorWriteGreen || colorWriteRed != colorWriteBlue)
            {
                std::ostringstream ss;
                ss << "Invalid ColorWrite values: values for rgb must match";
                throw GdtReadingException(ss.str());
            }

            m_base_statebits.loadBits[0] &= ~GFXS0_COLORWRITE_MASK;
            if (colorWriteRed == StateBitsEnabledStatus_e::ENABLED)
                m_base_statebits.loadBits[0] |= GFXS0_COLORWRITE_RGB;
            if (colorWriteAlpha == StateBitsEnabledStatus_e::ENABLED)
                m_base_statebits.loadBits[0] |= GFXS0_COLORWRITE_ALPHA;
        }

        void SetCullFace(const CullFace_e cullFace)
        {
            if (cullFace == CullFace_e::UNKNOWN)
            {
                std::ostringstream ss;
                ss << "Unknown cullFace values: \"\"";
                throw GdtReadingException(ss.str());
            }

            m_base_statebits.loadBits[0] &= ~GFXS0_CULL_MASK;

            if (cullFace == CullFace_e::FRONT)
            {
                m_base_statebits.loadBits[0] |= GFXS0_CULL_FRONT;
            }
            else if (cullFace == CullFace_e::BACK)
            {
                m_base_statebits.loadBits[0] |= GFXS0_CULL_BACK;
            }
            else
            {
                assert(cullFace == CullFace_e::NONE);
                m_base_statebits.loadBits[0] |= GFXS0_CULL_NONE;
            }
        }

        void SetDepthTest(const DepthTest_e depthTest)
        {
            m_base_statebits.loadBits[1] &= GFXS1_DEPTHTEST_MASK;

            switch (depthTest)
            {
            case DepthTest_e::LESS_EQUAL:
                m_base_statebits.loadBits[1] |= GFXS1_DEPTHTEST_LESSEQUAL;
                break;

            case DepthTest_e::LESS:
                m_base_statebits.loadBits[1] |= GFXS1_DEPTHTEST_LESS;
                break;

            case DepthTest_e::EQUAL:
                m_base_statebits.loadBits[1] |= GFXS1_DEPTHTEST_EQUAL;
                break;

            case DepthTest_e::ALWAYS:
                m_base_statebits.loadBits[1] |= GFXS1_DEPTHTEST_ALWAYS;
                break;

            case DepthTest_e::DISABLE:
                m_base_statebits.loadBits[1] |= GFXS1_DEPTHTEST_DISABLE;
                break;

            case DepthTest_e::UNKNOWN:
            default:
                std::ostringstream ss;
                ss << "Unknown depthTest values: \"\"";
                throw GdtReadingException(ss.str());
            }
        }

        void SetDepthWrite(const bool depthWrite)
        {
            m_base_statebits.loadBits[1] &= ~GFXS1_DEPTHWRITE;

            if (depthWrite)
                m_base_statebits.loadBits[1] |= GFXS1_DEPTHWRITE;
        }

        void SetPolygonOffset(const PolygonOffset_e polygonOffset)
        {
            if (polygonOffset == PolygonOffset_e::UNKNOWN)
            {
                std::ostringstream ss;
                ss << "Unknown polygonOffset values: \"\"";
                throw GdtReadingException(ss.str());
            }

            m_base_statebits.loadBits[1] &= ~GFXS1_POLYGON_OFFSET_MASK;
            m_base_statebits.loadBits[1] |= ((static_cast<unsigned>(polygonOffset) - 1) >> GFXS1_POLYGON_OFFSET_SHIFT) & GFXS1_POLYGON_OFFSET_MASK;
        }

        void FinalizeMaterial() const
        {
            if (!m_textures.empty())
            {
                m_material->textureTable = static_cast<MaterialTextureDef*>(m_memory->Alloc(sizeof(MaterialTextureDef) * m_textures.size()));
                m_material->textureCount = static_cast<unsigned char>(m_textures.size());
                memcpy(m_material->textureTable, m_textures.data(), sizeof(MaterialTextureDef) * m_textures.size());
            }
            else
            {
                m_material->textureTable = nullptr;
                m_material->textureCount = 0u;
            }
        }

        static size_t GetIndexForString(const std::string& propertyName, const std::string& value, const char** validValuesArray, const size_t validValuesArraySize)
        {
            for (auto i = 0u; i < validValuesArraySize; i++)
            {
                if (validValuesArray[i] == value)
                    return i;
            }

            std::ostringstream ss;
            ss << "Unknown " << propertyName << " value: \"" << value << "\"";
            throw GdtReadingException(ss.str());
        }

        template <typename T>
        T ReadEnumProperty(const std::string& propertyName, const char** validValuesArray, const size_t validValuesArraySize) const
        {
            return static_cast<T>(GetIndexForString(propertyName, ReadStringProperty(propertyName), validValuesArray, validValuesArraySize));
        }

        MemoryManager* m_memory;
        IAssetLoadingManager* m_manager;
        std::vector<XAssetInfoGeneric*> m_dependencies;

        Material* m_material;
        GfxStateBits m_base_statebits;
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
