#include "AssetDumperPhysConstraints.h"

#include <cassert>

#include "Game/T6/InfoStringT6.h"

using namespace T6;

cspField_t AssetDumperPhysConstraints::phys_constraints_fields[]
{
    {"c1_type", offsetof(PhysConstraints, data[0].type), CFT_TYPE},
    {"c1_bone1_name", offsetof(PhysConstraints, data[0].target_bone1), CSPFT_STRING},
    {"c1_bone2_name", offsetof(PhysConstraints, data[0].target_bone2), CSPFT_STRING},
    {"c1_offsetX", offsetof(PhysConstraints, data[0].offset.x), CSPFT_FLOAT},
    {"c1_offsetY", offsetof(PhysConstraints, data[0].offset.y), CSPFT_FLOAT},
    {"c1_offsetZ", offsetof(PhysConstraints, data[0].offset.z), CSPFT_FLOAT},
    {"c1_timeout", offsetof(PhysConstraints, data[0].timeout), CSPFT_INT},
    {"c1_min_health", offsetof(PhysConstraints, data[0].min_health), CSPFT_INT},
    {"c1_max_health", offsetof(PhysConstraints, data[0].max_health), CSPFT_INT},
    {"c1_damp", offsetof(PhysConstraints, data[0].damp), CSPFT_FLOAT},
    {"c1_power", offsetof(PhysConstraints, data[0].power), CSPFT_FLOAT},
    {"c1_spin_scale", offsetof(PhysConstraints, data[0].spin_scale), CSPFT_FLOAT},
    {"c1_shakescalex", offsetof(PhysConstraints, data[0].scale.x), CSPFT_FLOAT},
    {"c1_shakescaley", offsetof(PhysConstraints, data[0].scale.y), CSPFT_FLOAT},
    {"c1_shakescalez", offsetof(PhysConstraints, data[0].scale.z), CSPFT_FLOAT},
    {"c1_min_angle", offsetof(PhysConstraints, data[0].minAngle), CSPFT_FLOAT},
    {"c1_max_angle", offsetof(PhysConstraints, data[0].maxAngle), CSPFT_FLOAT},
    {"c1_yaw", offsetof(PhysConstraints, data[0].scale.y), CSPFT_FLOAT},
    {"c1_pitch", offsetof(PhysConstraints, data[0].scale.x), CSPFT_FLOAT},
    {"c2_type", offsetof(PhysConstraints, data[1].type), CFT_TYPE},
    {"c2_bone1_name", offsetof(PhysConstraints, data[1].target_bone1), CSPFT_STRING},
    {"c2_bone2_name", offsetof(PhysConstraints, data[1].target_bone2), CSPFT_STRING},
    {"c2_offsetX", offsetof(PhysConstraints, data[1].offset.x), CSPFT_FLOAT},
    {"c2_offsetY", offsetof(PhysConstraints, data[1].offset.y), CSPFT_FLOAT},
    {"c2_offsetZ", offsetof(PhysConstraints, data[1].offset.z), CSPFT_FLOAT},
    {"c2_timeout", offsetof(PhysConstraints, data[1].timeout), CSPFT_INT},
    {"c2_min_health", offsetof(PhysConstraints, data[1].min_health), CSPFT_INT},
    {"c2_max_health", offsetof(PhysConstraints, data[1].max_health), CSPFT_INT},
    {"c2_damp", offsetof(PhysConstraints, data[1].damp), CSPFT_FLOAT},
    {"c2_power", offsetof(PhysConstraints, data[1].power), CSPFT_FLOAT},
    {"c2_spin_scale", offsetof(PhysConstraints, data[1].spin_scale), CSPFT_FLOAT},
    {"c2_shakescalex", offsetof(PhysConstraints, data[1].scale.x), CSPFT_FLOAT},
    {"c2_shakescaley", offsetof(PhysConstraints, data[1].scale.y), CSPFT_FLOAT},
    {"c2_shakescalez", offsetof(PhysConstraints, data[1].scale.z), CSPFT_FLOAT},
    {"c2_min_angle", offsetof(PhysConstraints, data[1].minAngle), CSPFT_FLOAT},
    {"c2_max_angle", offsetof(PhysConstraints, data[1].maxAngle), CSPFT_FLOAT},
    {"c2_yaw", offsetof(PhysConstraints, data[1].scale.y), CSPFT_FLOAT},
    {"c2_pitch", offsetof(PhysConstraints, data[1].scale.x), CSPFT_FLOAT},
    {"c3_type", offsetof(PhysConstraints, data[2].type), CFT_TYPE},
    {"c3_bone1_name", offsetof(PhysConstraints, data[2].target_bone1), CSPFT_STRING},
    {"c3_bone2_name", offsetof(PhysConstraints, data[2].target_bone2), CSPFT_STRING},
    {"c3_offsetX", offsetof(PhysConstraints, data[2].offset.x), CSPFT_FLOAT},
    {"c3_offsetY", offsetof(PhysConstraints, data[2].offset.y), CSPFT_FLOAT},
    {"c3_offsetZ", offsetof(PhysConstraints, data[2].offset.z), CSPFT_FLOAT},
    {"c3_timeout", offsetof(PhysConstraints, data[2].timeout), CSPFT_INT},
    {"c3_min_health", offsetof(PhysConstraints, data[2].min_health), CSPFT_INT},
    {"c3_max_health", offsetof(PhysConstraints, data[2].max_health), CSPFT_INT},
    {"c3_damp", offsetof(PhysConstraints, data[2].damp), CSPFT_FLOAT},
    {"c3_power", offsetof(PhysConstraints, data[2].power), CSPFT_FLOAT},
    {"c3_spin_scale", offsetof(PhysConstraints, data[2].spin_scale), CSPFT_FLOAT},
    {"c3_shakescalex", offsetof(PhysConstraints, data[2].scale.x), CSPFT_FLOAT},
    {"c3_shakescaley", offsetof(PhysConstraints, data[2].scale.y), CSPFT_FLOAT},
    {"c3_shakescalez", offsetof(PhysConstraints, data[2].scale.z), CSPFT_FLOAT},
    {"c3_min_angle", offsetof(PhysConstraints, data[2].minAngle), CSPFT_FLOAT},
    {"c3_max_angle", offsetof(PhysConstraints, data[2].maxAngle), CSPFT_FLOAT},
    {"c3_yaw", offsetof(PhysConstraints, data[2].scale.y), CSPFT_FLOAT},
    {"c3_pitch", offsetof(PhysConstraints, data[2].scale.x), CSPFT_FLOAT},
    {"c4_type", offsetof(PhysConstraints, data[3].type), CFT_TYPE},
    {"c4_bone1_name", offsetof(PhysConstraints, data[3].target_bone1), CSPFT_STRING},
    {"c4_bone2_name", offsetof(PhysConstraints, data[3].target_bone2), CSPFT_STRING},
    {"c4_offsetX", offsetof(PhysConstraints, data[3].offset.x), CSPFT_FLOAT},
    {"c4_offsetY", offsetof(PhysConstraints, data[3].offset.y), CSPFT_FLOAT},
    {"c4_offsetZ", offsetof(PhysConstraints, data[3].offset.z), CSPFT_FLOAT},
    {"c4_timeout", offsetof(PhysConstraints, data[3].timeout), CSPFT_INT},
    {"c4_min_health", offsetof(PhysConstraints, data[3].min_health), CSPFT_INT},
    {"c4_max_health", offsetof(PhysConstraints, data[3].max_health), CSPFT_INT},
    {"c4_damp", offsetof(PhysConstraints, data[3].damp), CSPFT_FLOAT},
    {"c4_power", offsetof(PhysConstraints, data[3].power), CSPFT_FLOAT},
    {"c4_spin_scale", offsetof(PhysConstraints, data[3].spin_scale), CSPFT_FLOAT},
    {"c4_shakescalex", offsetof(PhysConstraints, data[3].scale.x), CSPFT_FLOAT},
    {"c4_shakescaley", offsetof(PhysConstraints, data[3].scale.y), CSPFT_FLOAT},
    {"c4_shakescalez", offsetof(PhysConstraints, data[3].scale.z), CSPFT_FLOAT},
    {"c4_min_angle", offsetof(PhysConstraints, data[3].minAngle), CSPFT_FLOAT},
    {"c4_max_angle", offsetof(PhysConstraints, data[3].maxAngle), CSPFT_FLOAT},
    {"c4_yaw", offsetof(PhysConstraints, data[3].scale.y), CSPFT_FLOAT},
    {"c4_pitch", offsetof(PhysConstraints, data[3].scale.x), CSPFT_FLOAT},
};

namespace T6
{
    const char* s_constraintTypeNames[]
    {
        "none",
        "point",
        "distance",
        "hinge",
        "joint",
        "actuator",
        "fake_shake",
        "launch",
        "rope",
        "light",
    };

    class InfoStringFromPhysConstraintsConverter final : public InfoStringFromStructConverter
    {
    protected:
        void FillFromExtensionField(const cspField_t& field) override
        {
            switch (static_cast<constraintsFieldType_t>(field.iFieldType))
            {
            case CFT_TYPE:
                FillFromEnumInt(std::string(field.szName), field.iOffset, s_constraintTypeNames, _countof(s_constraintTypeNames));
                break;

            case CFT_NUM:
            default:
                assert(false);
                break;
            }
        }

    public:
        InfoStringFromPhysConstraintsConverter(const PhysConstraints* structure, const cspField_t* fields, const size_t fieldCount, std::function<std::string(scr_string_t)> scriptStringValueCallback)
            : InfoStringFromStructConverter(structure, fields, fieldCount, std::move(scriptStringValueCallback))
        {
        }
    };
}

bool AssetDumperPhysConstraints::ShouldDump(XAssetInfo<PhysConstraints>* asset)
{
    return true;
}

std::string AssetDumperPhysConstraints::GetFileNameForAsset(Zone* zone, XAssetInfo<PhysConstraints>* asset)
{
    return "physconstraints/" + asset->m_name;
}

void AssetDumperPhysConstraints::DumpAsset(Zone* zone, XAssetInfo<PhysConstraints>* asset, FileAPI::File* out)
{
    assert(asset->Asset()->count <= 4);

    InfoStringFromPhysConstraintsConverter converter(asset->Asset(), phys_constraints_fields, _countof(phys_constraints_fields), [asset](const scr_string_t scrStr) -> std::string
    {
        assert(scrStr < asset->m_zone->m_script_strings.size());
        if (scrStr >= asset->m_zone->m_script_strings.size())
            return "";

        return asset->m_zone->m_script_strings[scrStr];
    });

    const auto infoString = converter.Convert();
    const auto stringValue = infoString.ToString("PHYSCONSTRAINTS");
    out->Write(stringValue.c_str(), 1, stringValue.length());
}
