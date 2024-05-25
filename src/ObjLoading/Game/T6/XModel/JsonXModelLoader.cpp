#include "JsonXModelLoader.h"

#include "Game/T6/CommonT6.h"
#include "Game/T6/Json/JsonXModel.h"
#include "Utils/StringUtils.h"
#include "XModel/Gltf/GltfBinInput.h"
#include "XModel/Gltf/GltfLoader.h"
#include "XModel/Gltf/GltfTextInput.h"
#include "XModel/XModelCommon.h"

#include <filesystem>
#include <format>
#include <iostream>
#include <nlohmann/json.hpp>
#include <vector>

using namespace nlohmann;
using namespace T6;

namespace fs = std::filesystem;

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
                std::cerr << std::format("Tried to load xmodel \"{}\" but did not find expected type material of version 1\n", xmodel.name);
                return false;
            }

            const auto jXModel = jRoot.get<JsonXModel>();
            return CreateXModelFromJson(jXModel, xmodel);
        }

    private:
        static void PrintError(const XModel& xmodel, const std::string& message)
        {
            std::cerr << std::format("Cannot load xmodel \"{}\": {}\n", xmodel.name, message);
        }

        static std::unique_ptr<XModelCommon> LoadModelByExtension(std::istream& stream, const std::string& extension)
        {
            if (extension == ".glb")
            {
                gltf::BinInput input;
                if (!input.ReadGltfData(stream))
                    return nullptr;

                const auto loader = gltf::Loader::CreateLoader(&input);
                return loader->Load();
            }

            if (extension == ".gltf")
            {
                gltf::TextInput input;
                if (!input.ReadGltfData(stream))
                    return nullptr;

                const auto loader = gltf::Loader::CreateLoader(&input);
                return loader->Load();
            }

            return nullptr;
        }

        bool ApplyCommonBonesToXModel(const JsonXModelLod& jLod, XModel& xmodel, unsigned lodNumber, const XModelCommon& common) const
        {
            if (common.m_bones.empty())
                return true;

            const auto boneCount = common.m_bones.size();
            constexpr auto maxBones = std::numeric_limits<decltype(XModel::numBones)>::max();
            if (boneCount > maxBones)
            {
                PrintError(xmodel, std::format("Model \"{}\" for lod {} contains too many bones ({} -> max={})", jLod.file, lodNumber, boneCount, maxBones));
                return false;
            }

            xmodel.numRootBones = 0u;
            xmodel.numBones = 0u;
            for (const auto& bone : common.m_bones)
            {
                if (!bone.parentIndex)
                {
                    // Make sure root bones are at the beginning
                    assert(xmodel.numRootBones == xmodel.numBones);
                    xmodel.numRootBones++;
                }

                xmodel.numBones++;
            }

            xmodel.boneNames = m_memory.Alloc<ScriptString>(xmodel.numBones);
            xmodel.partClassification = m_memory.Alloc<char>(xmodel.numBones);
            xmodel.baseMat = m_memory.Alloc<DObjAnimMat>(xmodel.numBones);
            xmodel.boneInfo = m_memory.Alloc<XBoneInfo>(xmodel.numBones);

            if (xmodel.numBones > xmodel.numRootBones)
            {
                xmodel.parentList = m_memory.Alloc<unsigned char>(xmodel.numBones - xmodel.numRootBones);
                xmodel.trans = m_memory.Alloc<float[4]>(xmodel.numBones - xmodel.numRootBones);
                xmodel.quats = m_memory.Alloc<uint16_t[4]>(xmodel.numBones - xmodel.numRootBones);
            }
            else
            {
                xmodel.parentList = nullptr;
                xmodel.trans = nullptr;
                xmodel.quats = nullptr;
            }
        }

        bool LoadLod(const JsonXModelLod& jLod, XModel& xmodel, unsigned lodNumber) const
        {
            const auto file = m_manager.GetAssetLoadingContext()->m_raw_search_path->Open(jLod.file);
            if (!file.IsOpen())
            {
                PrintError(xmodel, std::format("Failed to open file for lod {}: \"{}\"", lodNumber, jLod.file));
                return false;
            }

            auto extension = fs::path(jLod.file).extension().string();
            utils::MakeStringLowerCase(extension);

            const auto common = LoadModelByExtension(*file.m_stream, extension);
            if (!common)
            {
                PrintError(xmodel, std::format("Failure while trying to load model for lod {}: \"{}\"", lodNumber, jLod.file));
                return false;
            }

            if (lodNumber == 0u)
            {
                if (!ApplyCommonBonesToXModel(jLod, xmodel, lodNumber, *common))
                    return false;
            }
            // TODO: Verify bones if not lod 0

            // TODO: Apply common XModel vertex data

            return true;
        }

        bool CreateXModelFromJson(const JsonXModel& jXModel, XModel& xmodel) const
        {
            auto lodNumber = 0u;
            for (const auto& jLod : jXModel.lods)
                LoadLod(jLod, xmodel, lodNumber++);

            if (jXModel.collLod && jXModel.collLod.value() >= 0)
            {
                if (static_cast<unsigned>(jXModel.collLod.value()) >= jXModel.lods.size())
                {
                    PrintError(xmodel, "Collision lod is not a valid lod");
                    return false;
                }
                xmodel.collLod = static_cast<int16_t>(jXModel.collLod.value());
            }
            else
                xmodel.collLod = -1;

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
