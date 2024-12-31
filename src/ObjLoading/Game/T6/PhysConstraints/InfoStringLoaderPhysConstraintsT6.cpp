#include "InfoStringLoaderPhysConstraintsT6.h"

#include "Game/T6/InfoString/InfoStringToStructConverter.h"
#include "Game/T6/PhysConstraints/PhysConstraintsFields.h"
#include "Game/T6/T6.h"

#include <cassert>
#include <cstring>
#include <format>
#include <iostream>
#include <limits>

using namespace T6;

namespace
{
    class InfoStringToPhysConstraintsConverter final : public InfoStringToStructConverter
    {
    protected:
        bool ConvertExtensionField(const cspField_t& field, const std::string& value) override
        {
            switch (static_cast<constraintsFieldType_t>(field.iFieldType))
            {
            case CFT_TYPE:
                return ConvertEnumInt(field.szName, value, field.iOffset, s_constraintTypeNames, std::extent_v<decltype(s_constraintTypeNames)>);

            default:
                assert(false);
                return false;
            }
        }

    public:
        InfoStringToPhysConstraintsConverter(const InfoString& infoString,
                                             PhysConstraints& physConstraints,
                                             ZoneScriptStrings& zoneScriptStrings,
                                             MemoryManager& memory,
                                             AssetCreationContext& context,
                                             AssetRegistration<AssetPhysConstraints>& registration,
                                             const cspField_t* fields,
                                             const size_t fieldCount)
            : InfoStringToStructConverter(infoString, &physConstraints, zoneScriptStrings, memory, context, registration, fields, fieldCount)
        {
        }
    };

    void CalculatePhysConstraintsFields(PhysConstraints& physConstraints, ZoneScriptStrings& scriptStrings)
    {
        for (auto& constraint : physConstraints.data)
        {
            constraint.targetname = scriptStrings.AddOrGetScriptString("");
            constraint.target_ent1 = scriptStrings.AddOrGetScriptString("");
            constraint.target_ent2 = scriptStrings.AddOrGetScriptString("");
            constraint.attach_point_type1 = ATTACH_POINT_BONE;
            constraint.attach_point_type2 = ATTACH_POINT_BONE;
            constraint.target_bone1 = "";
            constraint.target_bone2 = "";
        }

        // count
        {
            auto foundEnd = false;
            for (auto i = 0u; i < std::extent_v<decltype(PhysConstraints::data)>; i++)
            {
                if (physConstraints.data[i].type == CONSTRAINT_NONE)
                {
                    foundEnd = true;
                    physConstraints.count = i;
                    break;
                }
            }

            if (!foundEnd)
                physConstraints.count = std::extent_v<decltype(PhysConstraints::data)>;
        }
    }
} // namespace

InfoStringLoaderPhysConstraints::InfoStringLoaderPhysConstraints(MemoryManager& memory, ISearchPath& searchPath, Zone& zone)
    : m_memory(memory),
      m_search_path(searchPath),
      m_zone(zone)
{
}

AssetCreationResult InfoStringLoaderPhysConstraints::CreateAsset(const std::string& assetName, const InfoString& infoString, AssetCreationContext& context)
{
    auto* physConstraints = m_memory.Alloc<PhysConstraints>();
    physConstraints->name = m_memory.Dup(assetName.c_str());

    AssetRegistration<AssetPhysConstraints> registration(assetName, physConstraints);
    InfoStringToPhysConstraintsConverter converter(infoString,
                                                   *physConstraints,
                                                   m_zone.m_script_strings,
                                                   m_memory,
                                                   context,
                                                   registration,
                                                   phys_constraints_fields,
                                                   std::extent_v<decltype(phys_constraints_fields)>);
    if (!converter.Convert())
    {
        std::cerr << std::format("Failed to parse phys constraints: \"{}\"\n", assetName);
        return AssetCreationResult::Failure();
    }

    CalculatePhysConstraintsFields(*physConstraints, m_zone.m_script_strings);
    registration.AddScriptString(m_zone.m_script_strings.AddOrGetScriptString(""));

    return AssetCreationResult::Success(context.AddAsset(std::move(registration)));
}
