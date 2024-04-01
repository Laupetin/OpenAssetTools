#include "JsonXModelLoader.h"

#include "Game/T6/CommonT6.h"
#include "Game/T6/Json/JsonXModel.h"

#include <format>
#include <iostream>
#include <nlohmann/json.hpp>
#include <vector>

using namespace nlohmann;
using namespace T6;

namespace
{
    class JsonLoader
    {
    public:
        JsonLoader(std::istream& stream, MemoryManager& memory, IAssetLoadingManager& manager, std::set<XAssetInfoGeneric*>& dependencies)
            : m_stream(stream),
              m_memory(memory),
              m_manager(manager),
              m_dependencies(dependencies)

        {
        }

        bool Load(XModel& xmodel) const
        {
            const auto jRoot = json::parse(m_stream);
            std::string type;
            unsigned version;

            jRoot.at("_type").get_to(type);
            jRoot.at("_version").get_to(version);

            if (type != "xmodel" || version != 1u)
            {
                std::cerr << "Tried to load xmodel \"" << xmodel.name << "\" but did not find expected type material of version 1\n";
                return false;
            }

            const auto jXModel = jRoot.get<JsonXModel>();
            return CreateXModelFromJson(jXModel, xmodel);
        }

    private:
        static void PrintError(const XModel& xmodel, const std::string& message)
        {
            std::cerr << "Cannot load xmodel \"" << xmodel.name << "\": " << message << "\n";
        }

        bool CreateXModelFromJson(const JsonXModel& jXModel, XModel& xmodel) const
        {
            xmodel.collLod = static_cast<uint16_t>(jXModel.collLod);

            if (jXModel.physPreset)
            {
                auto* physPreset = m_manager.LoadDependency<AssetPhysPreset>(jXModel.physPreset.value());
                if (!physPreset)
                {
                    PrintError(xmodel, "Could not find phys preset");
                    return false;
                }
                m_dependencies.emplace(physPreset);
                xmodel.physPreset = physPreset->Asset();
            }
            else
            {
                xmodel.physPreset = nullptr;
            }

            if (jXModel.physConstraints)
            {
                auto* physConstraints = m_manager.LoadDependency<AssetPhysConstraints>(jXModel.physConstraints.value());
                if (!physConstraints)
                {
                    PrintError(xmodel, "Could not find phys constraints");
                    return false;
                }
                m_dependencies.emplace(physConstraints);
                xmodel.physConstraints = physConstraints->Asset();
            }
            else
            {
                xmodel.physConstraints = nullptr;
            }

            xmodel.flags = jXModel.flags;
            xmodel.lightingOriginOffset.x = jXModel.lightingOriginOffset.x;
            xmodel.lightingOriginOffset.y = jXModel.lightingOriginOffset.y;
            xmodel.lightingOriginOffset.z = jXModel.lightingOriginOffset.z;
            xmodel.lightingOriginRange = jXModel.lightingOriginRange;

            return true;
        }

        std::istream& m_stream;
        MemoryManager& m_memory;
        IAssetLoadingManager& m_manager;
        std::set<XAssetInfoGeneric*>& m_dependencies;
    };
} // namespace

namespace T6
{
    bool LoadXModelAsJson(
        std::istream& stream, XModel& xmodel, MemoryManager* memory, IAssetLoadingManager* manager, std::vector<XAssetInfoGeneric*>& dependencies)
    {
        std::set<XAssetInfoGeneric*> dependenciesSet;
        const JsonLoader loader(stream, *memory, *manager, dependenciesSet);

        dependencies.assign(dependenciesSet.cbegin(), dependenciesSet.cend());

        return loader.Load(xmodel);
    }
} // namespace T6
