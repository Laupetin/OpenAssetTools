#include "BSPCreator.h"

#include "BSPUtil.h"
#include "Utils/StringUtils.h"
#include "XModel/Gltf/GltfBinInput.h"
#include "XModel/Gltf/GltfTextInput.h"
#include "XModel/Gltf/Internal/GltfAccessor.h"
#include "XModel/Gltf/Internal/GltfBuffer.h"
#include "XModel/Gltf/Internal/GltfBufferView.h"
#include "XModel/Gltf/JsonGltf.h"
#include "XModel/Tangentspace.h"

#pragma warning(push, 0)
#include <Eigen>
#pragma warning(pop)

#include <deque>
#include <exception>
#include <format>
#include <iostream>
#include <limits>
#include <numbers>
#include <string>

using namespace BSPFlags;

namespace
{
    struct AccessorsForVertex
    {
        unsigned m_position_accessor;
        unsigned m_normal_accessor;
        std::optional<unsigned> m_color_accessor;
        std::optional<unsigned> m_uv_accessor;
        unsigned m_index_accessor;
    };

    void RhcToLhcQuaternion(float (&coords)[4])
    {
        const float two[4]{coords[0], coords[1], coords[2], coords[3]};

        coords[0] = two[0];
        coords[1] = -two[2];
        coords[2] = two[1];
        coords[3] = two[3];
    }

    void RhcToLhcCoordinates(float (&coords)[3])
    {
        const float two[3]{coords[0], coords[1], coords[2]};

        coords[0] = two[0];
        coords[1] = -two[2];
        coords[2] = two[1];
    }

    void RhcToLhcIndices(unsigned (&indices)[3])
    {
        const unsigned two[3]{indices[0], indices[1], indices[2]};

        indices[0] = two[2];
        indices[1] = two[1];
        indices[2] = two[0];
    }
} // namespace

namespace
{
    using namespace BSP;
    using namespace gltf;

    class GltfLoadException final : std::exception
    {
    public:
        explicit GltfLoadException(std::string message)
            : m_message(std::move(message))
        {
        }

        [[nodiscard]] const std::string& Str() const
        {
            return m_message;
        }

        [[nodiscard]] const char* what() const noexcept override
        {
            return m_message.c_str();
        }

    private:
        std::string m_message;
    };

    class BSPLoader
    {
    private:
        BSPData* m_bsp;
        BSPWorld* m_curr_bsp_world;
        bool m_is_world_gfx;
        size_t m_emptyMaterialIndex;

        std::vector<std::unique_ptr<Accessor>> m_accessors;
        std::vector<std::unique_ptr<BufferView>> m_buffer_views;
        std::vector<std::unique_ptr<Buffer>> m_buffers;

        std::string getWorldTypeName()
        {
            return m_is_world_gfx ? "gfx" : "col";
        }

        std::optional<Accessor*> GetAccessorForIndex(const char* attributeName,
                                                     const std::optional<unsigned> index,
                                                     std::initializer_list<JsonAccessorType> allowedAccessorTypes,
                                                     std::initializer_list<JsonAccessorComponentType> allowedAccessorComponentTypes) const
        {
            if (!index)
                return std::nullopt;

            if (*index > m_accessors.size())
                throw GltfLoadException(std::format("Index for {} accessor out of bounds", attributeName));

            auto* accessor = m_accessors[*index].get();

            const auto maybeType = accessor->GetType();
            if (maybeType)
            {
                if (std::ranges::find(allowedAccessorTypes, *maybeType) == allowedAccessorTypes.end())
                    throw GltfLoadException(std::format("Accessor for {} has unsupported type {}", attributeName, static_cast<unsigned>(*maybeType)));
            }

            const auto maybeComponentType = accessor->GetComponentType();
            if (maybeComponentType)
            {
                if (std::ranges::find(allowedAccessorComponentTypes, *maybeComponentType) == allowedAccessorComponentTypes.end())
                    throw GltfLoadException(
                        std::format("Accessor for {} has unsupported component type {}", attributeName, static_cast<unsigned>(*maybeComponentType)));
            }

            return accessor;
        }

        static void VerifyAccessorVertexCount(const char* accessorType, const Accessor* accessor, const size_t vertexCount)
        {
            if (accessor->GetCount() != vertexCount)
                throw GltfLoadException(std::format("Element count of {} accessor does not match expected vertex count of {}", accessorType, vertexCount));
        }

        using Transform3f = Eigen::Transform<float, 3, Eigen::Affine>;

        Eigen::Matrix4f createNodeMatrix(const gltf::JsonNode& node)
        {
            if (node.matrix)
                return Eigen::Matrix4f({
                    {(*node.matrix)[0], (*node.matrix)[4], (*node.matrix)[8],  (*node.matrix)[12]},
                    {(*node.matrix)[1], (*node.matrix)[5], (*node.matrix)[9],  (*node.matrix)[13]},
                    {(*node.matrix)[2], (*node.matrix)[6], (*node.matrix)[10], (*node.matrix)[14]},
                    {(*node.matrix)[3], (*node.matrix)[7], (*node.matrix)[11], (*node.matrix)[15]}
                });

            float localTranslation[3];
            float localRotation[4];
            float localScale[3];
            if (node.translation)
            {
                localTranslation[0] = (*node.translation)[0];
                localTranslation[1] = (*node.translation)[1];
                localTranslation[2] = (*node.translation)[2];
            }
            else
            {
                localTranslation[0] = 0.0f;
                localTranslation[1] = 0.0f;
                localTranslation[2] = 0.0f;
            }

            if (node.rotation)
            {
                localRotation[0] = (*node.rotation)[0];
                localRotation[1] = (*node.rotation)[1];
                localRotation[2] = (*node.rotation)[2];
                localRotation[3] = (*node.rotation)[3];
            }
            else
            {
                localRotation[0] = 0.0f;
                localRotation[1] = 0.0f;
                localRotation[2] = 0.0f;
                localRotation[3] = 1.0f;
            }

            if (node.scale)
            {
                localScale[0] = (*node.scale)[0];
                localScale[1] = (*node.scale)[1];
                localScale[2] = (*node.scale)[2];
            }
            else
            {
                localScale[0] = 1.0f;
                localScale[1] = 1.0f;
                localScale[2] = 1.0f;
            }

            Eigen::Vector3f translation(localTranslation[0], localTranslation[1], localTranslation[2]);
            Eigen::Quaternionf rotation(localRotation[3], localRotation[0], localRotation[1], localRotation[2]); // GLTF is XYZW, Eigen is WXYZ
            Eigen::Vector3f scale(localScale[0], localScale[1], localScale[2]);

            Transform3f T;
            T = T.fromPositionOrientationScale(translation, rotation, scale);
            return T.matrix();
        }

        unsigned CreateSurface(const AccessorsForVertex& accessorsForVertex,
                               Eigen::Matrix4f& nodeMatrix,
                               size_t materialIndex,
                               bool convertWorldToLocalPos,
                               BSPSurface& out_surface)
        {
            // clang-format off
            const auto* positionAccessor = GetAccessorForIndex(
                "POSITION",
                accessorsForVertex.m_position_accessor,
                { JsonAccessorType::VEC3 },
                { JsonAccessorComponentType::FLOAT }
            ).value_or(nullptr);
            // clang-format on
            assert(positionAccessor != nullptr);

            const auto vertexCount = positionAccessor->GetCount();
            NullAccessor nullAccessor(vertexCount);
            OnesAccessor onesAccessor(vertexCount);

            // clang-format off
            const auto* normalAccessor = GetAccessorForIndex(
                "NORMAL",
                accessorsForVertex.m_normal_accessor,
                { JsonAccessorType::VEC3 },
                { JsonAccessorComponentType::FLOAT }
            ).value_or(nullptr);
            VerifyAccessorVertexCount("NORMAL", normalAccessor, vertexCount);
            assert(normalAccessor != nullptr);

            const auto* uvAccessor = GetAccessorForIndex(
                "TEXCOORD_0",
                accessorsForVertex.m_uv_accessor,
                { JsonAccessorType::VEC2 },
                { JsonAccessorComponentType::FLOAT, JsonAccessorComponentType::UNSIGNED_BYTE, JsonAccessorComponentType::UNSIGNED_SHORT }
            ).value_or(&nullAccessor);
            VerifyAccessorVertexCount("TEXCOORD_0", uvAccessor, vertexCount);

            const auto* colorAccessor = GetAccessorForIndex(
                "COLOR_0",
                accessorsForVertex.m_color_accessor,
                { JsonAccessorType::VEC3, JsonAccessorType::VEC4 },
                { JsonAccessorComponentType::FLOAT, JsonAccessorComponentType::UNSIGNED_BYTE, JsonAccessorComponentType::UNSIGNED_SHORT }
            ).value_or(&onesAccessor);
            VerifyAccessorVertexCount("COLOR_0", colorAccessor, vertexCount);

            const auto* indexAccessor = GetAccessorForIndex(
                "INDICES",
                accessorsForVertex.m_index_accessor,
                { JsonAccessorType::SCALAR },
                { JsonAccessorComponentType::UNSIGNED_BYTE, JsonAccessorComponentType::UNSIGNED_SHORT, JsonAccessorComponentType::UNSIGNED_INT }
            ).value_or(nullptr);
            assert(indexAccessor != nullptr);
            // clang-format on

            const auto indexCount = indexAccessor->GetCount();
            if (indexCount % 3 != 0)
                throw GltfLoadException("Index count must be dividable by 3 for triangles");
            const auto faceCount = indexCount / 3u;
            if (faceCount > UINT16_MAX)
                throw GltfLoadException("Face count exceeded the UINT16_MAX");

            out_surface.vertexCount = static_cast<uint16_t>(vertexCount);
            out_surface.triCount = static_cast<uint16_t>(faceCount);
            out_surface.indexOfFirstIndex = static_cast<int>(m_curr_bsp_world->indices.size());
            out_surface.indexOfFirstVertex = static_cast<int>(m_curr_bsp_world->vertices.size());
            out_surface.materialIndex = materialIndex;
            vec4_t materialColor = m_curr_bsp_world->materials.at(materialIndex).materialColour;

            Eigen::Vector4f tempPosition(0, 0, 0, 1.0f);
            Eigen::Vector4f transformedPosition = nodeMatrix * tempPosition;
            vec3_t surfaceOrigin;
            surfaceOrigin.x = transformedPosition.x();
            surfaceOrigin.y = transformedPosition.y();
            surfaceOrigin.z = transformedPosition.z();
            RhcToLhcCoordinates(surfaceOrigin.v);

            for (auto faceIndex = 0u; faceIndex < faceCount; faceIndex++)
            {
                unsigned indices[3];
                if (!indexAccessor->GetUnsigned(faceIndex * 3u + 0u, indices[0]) || !indexAccessor->GetUnsigned(faceIndex * 3u + 1u, indices[1])
                    || !indexAccessor->GetUnsigned(faceIndex * 3u + 2u, indices[2]))
                {
                    assert(false);
                }
                if (indices[0] > UINT16_MAX || indices[1] > UINT16_MAX || indices[2] > UINT16_MAX)
                    throw GltfLoadException("Index number exceeded the UINT16_MAX");

                RhcToLhcIndices(indices);
                m_curr_bsp_world->indices.emplace_back(static_cast<uint16_t>(indices[0]));
                m_curr_bsp_world->indices.emplace_back(static_cast<uint16_t>(indices[1]));
                m_curr_bsp_world->indices.emplace_back(static_cast<uint16_t>(indices[2]));
            }

            const auto vertexOffset = static_cast<unsigned>(m_curr_bsp_world->vertices.size());
            m_curr_bsp_world->vertices.reserve(vertexOffset + vertexCount);
            for (auto vertexIndex = 0u; vertexIndex < vertexCount; vertexIndex++)
            {
                BSPVertex vertex;

                if (!positionAccessor->GetFloatVec3(vertexIndex, vertex.pos.v) || !normalAccessor->GetFloatVec3(vertexIndex, vertex.normal.v)
                    || !colorAccessor->GetFloatVec4(vertexIndex, vertex.color.v) || !uvAccessor->GetFloatVec2(vertexIndex, vertex.texCoord.v))
                {
                    assert(false);
                }

                vertex.color.x *= materialColor.x;
                vertex.color.y *= materialColor.y;
                vertex.color.z *= materialColor.z;
                vertex.color.w *= materialColor.w;

                Eigen::Vector4f position(vertex.pos.x, vertex.pos.y, vertex.pos.z, 1.0f);
                Eigen::Vector4f transformedPosition = nodeMatrix * position;
                vertex.pos.x = transformedPosition.x();
                vertex.pos.y = transformedPosition.y();
                vertex.pos.z = transformedPosition.z();

                RhcToLhcCoordinates(vertex.pos.v);
                RhcToLhcCoordinates(vertex.normal.v);

                if (convertWorldToLocalPos)
                {
                    vertex.pos.x -= surfaceOrigin.x;
                    vertex.pos.y -= surfaceOrigin.y;
                    vertex.pos.z -= surfaceOrigin.z;
                }

                m_curr_bsp_world->vertices.emplace_back(vertex);
            }

            // generate tangent and binormal vectors
            tangent_space::VertexData vertexData{
                &m_curr_bsp_world->vertices[out_surface.indexOfFirstVertex].pos,
                sizeof(BSPVertex),
                &m_curr_bsp_world->vertices[out_surface.indexOfFirstVertex].normal,
                sizeof(BSPVertex),
                &m_curr_bsp_world->vertices[out_surface.indexOfFirstVertex].texCoord,
                sizeof(BSPVertex),
                &m_curr_bsp_world->vertices[out_surface.indexOfFirstVertex].tangent,
                sizeof(BSPVertex),
                &m_curr_bsp_world->vertices[out_surface.indexOfFirstVertex].binormal,
                sizeof(BSPVertex),
                &m_curr_bsp_world->indices[out_surface.indexOfFirstIndex],
            };
            tangent_space::CalculateTangentSpace(vertexData, faceCount, vertexCount);

            return vertexOffset;
        }

        bool addLightNode(const gltf::JsonNode& node)
        {
            assert(m_is_world_gfx);
            assert(node.extensions);
            assert(node.extensions->KHR_lights_punctual);

            Eigen::Matrix4f nodeMatrix = createNodeMatrix(node);

            int lightIndex = node.extensions->KHR_lights_punctual->light;

            assert(lightIndex >= 0);
            if (m_bsp->lights[lightIndex].hasPosBeenSet == true)
                con::warn("Internal error, multiple nodes reference the same light. Light positions/rotations are likely incorrect.");

            Eigen::Vector4f position(0, 0, 0, 1.0f);
            Eigen::Vector4f transformedPosition = nodeMatrix * position;
            m_bsp->lights[lightIndex].pos = vec3_t{transformedPosition.x(), transformedPosition.y(), transformedPosition.z()};
            RhcToLhcCoordinates(m_bsp->lights[lightIndex].pos.v);

            // GLTF spec uses +Y up and the default light direction is straight down
            Eigen::Vector3f defaultDirection(0.0f, -1.0f, 0.0f);
            Eigen::Affine3f affineTransform(nodeMatrix);
            Eigen::Matrix3f rotationMatrix = affineTransform.rotation();
            Eigen::Vector3f outputDirection = rotationMatrix * defaultDirection;
            outputDirection.normalize();
            m_bsp->lights[lightIndex].direction = vec3_t{outputDirection.x(), outputDirection.y(), outputDirection.z()};

            m_bsp->lights[lightIndex].hasPosBeenSet = true;

            return true;
        }

        size_t createMaterialWithFlags(size_t originalMaterialIdx, const std::string& flags, bool& isNoDrawFlagSet)
        {
            BSPMaterial newMaterial = m_curr_bsp_world->materials.at(originalMaterialIdx);

            bool matchedAnyFlag = false;
            std::vector<std::string> flagStrVec = utils::StringSplit(flags, ',');
            for (std::string& flag : flagStrVec)
            {
                bool foundMatchingName = false;
                utils::MakeStringLowerCase(flag);
                utils::StringTrim(flag);
                size_t typeNameCount = std::extent<decltype(BSPFlags::materialFlags)>::value;
                for (size_t typeIdx = 0; typeIdx < typeNameCount; typeIdx++)
                {
                    BSPFlags::SurfaceType surfType = BSPFlags::materialFlags[typeIdx];
                    if (!flag.compare(BSPFlags::surfaceTypeToNameMap[surfType]))
                    {
                        BSPFlags::s_SurfaceTypeFlags flags = BSPFlags::surfaceTypeToFlagMap[surfType];
                        newMaterial.surfaceFlags |= flags.surfaceFlags;
                        newMaterial.contentFlags |= flags.contentFlags;
                        foundMatchingName = true;
                        matchedAnyFlag = true;
                        break;
                    }
                }
                if (!foundMatchingName)
                    con::warn("material {} has invalid flag name: {}", newMaterial.materialName, flag);
            }
            if (!matchedAnyFlag)
                return originalMaterialIdx;

            // the first content flag bit must be set to 1 for the surface to have collision
            if ((newMaterial.surfaceFlags & BSPFlags::surfaceTypeToFlagMap[BSPFlags::SURF_TYPE_NONSOLID].surfaceFlags) != 0)
                newMaterial.contentFlags &= 0xFFFFFFFE;
            else
                newMaterial.contentFlags |= 1;

            if ((newMaterial.surfaceFlags & BSPFlags::surfaceTypeToFlagMap[BSPFlags::SURF_TYPE_NODRAW].surfaceFlags) != 0)
                isNoDrawFlagSet = true;
            else
                isNoDrawFlagSet = false;

            size_t newMaterialIndex = m_curr_bsp_world->materials.size();
            m_curr_bsp_world->materials.emplace_back(newMaterial);
            return newMaterialIndex;
        }

        bool addMeshNode(const JsonRoot& jRoot, const gltf::JsonNode& node)
        {
            assert(node.mesh);
            assert(jRoot.meshes);

            Eigen::Matrix4f nodeMatrix = createNodeMatrix(node);

            const auto& mesh = jRoot.meshes.value()[node.mesh.value()];
            for (const auto& primitive : mesh.primitives)
            {
                if (!primitive.indices)
                    throw GltfLoadException("Requires primitives indices");
                if (primitive.mode.value_or(JsonMeshPrimitivesMode::TRIANGLES) != JsonMeshPrimitivesMode::TRIANGLES)
                    throw GltfLoadException("Only triangles are supported");
                if (!primitive.attributes.POSITION)
                    throw GltfLoadException("Requires primitives attribute POSITION");
                if (!primitive.attributes.NORMAL)
                    throw GltfLoadException("Requires primitives attribute NORMAL");

                const AccessorsForVertex accessorsForVertex{
                    .m_position_accessor = *primitive.attributes.POSITION,
                    .m_normal_accessor = *primitive.attributes.NORMAL,
                    .m_color_accessor = primitive.attributes.COLOR_0,
                    .m_uv_accessor = primitive.attributes.TEXCOORD_0,
                    .m_index_accessor = *primitive.indices,
                };

                size_t materialIndex;
                if (primitive.material)
                {
                    size_t originalMaterialIdx = *primitive.material;
                    if (node.extras && node.extras->contains("flags"))
                    {
                        bool isNoDrawFlagSet = false;
                        materialIndex = createMaterialWithFlags(originalMaterialIdx, node.extras->at("flags"), isNoDrawFlagSet);
                        if (isNoDrawFlagSet && m_is_world_gfx)
                            continue; // noDraw flag doesn't work, so remove the surface from the graphics data instead
                    }
                    else
                        materialIndex = originalMaterialIdx;
                }
                else
                    materialIndex = m_emptyMaterialIndex;

                BSPSurface surface;
                CreateSurface(accessorsForVertex, nodeMatrix, materialIndex, false, surface);
                m_curr_bsp_world->staticSurfaces.emplace_back(surface);
            }

            return true;
        }

        void calculateXmodelBounds(BSPXModel& xmodel, std::optional<int> meshIndex, Eigen::Matrix4f& nodeMatrix, const JsonRoot& jRoot)
        {
            if (meshIndex)
            {
                xmodel.areBoundsValid = true;
                Eigen::Vector4f position(0, 0, 0, 1.0f);
                Eigen::Vector4f transformedPosition = nodeMatrix * position;
                xmodel.mins.x = transformedPosition.x();
                xmodel.mins.y = transformedPosition.y();
                xmodel.mins.z = transformedPosition.z();
                xmodel.maxs.x = transformedPosition.x();
                xmodel.maxs.y = transformedPosition.y();
                xmodel.maxs.z = transformedPosition.z();
                RhcToLhcCoordinates(xmodel.mins.v);
                RhcToLhcCoordinates(xmodel.maxs.v);

                const auto& mesh = jRoot.meshes.value()[*meshIndex];
                for (size_t primIdx = 0; primIdx < mesh.primitives.size(); primIdx++)
                {
                    const auto& primitive = mesh.primitives.at(primIdx);

                    if (!primitive.attributes.POSITION)
                        throw GltfLoadException("Requires primitives attribute POSITION");

                    // clang-format off
                     const auto* positionAccessor = GetAccessorForIndex(
                         "POSITION",
                         primitive.attributes.POSITION,
                         { JsonAccessorType::VEC3 },
                         { JsonAccessorComponentType::FLOAT }
                     ).value_or(nullptr);
                    // clang-format on
                    assert(positionAccessor != nullptr);

                    for (size_t vertexIndex = 0u; vertexIndex < positionAccessor->GetCount(); vertexIndex++)
                    {
                        vec3_t vertex;
                        if (!positionAccessor->GetFloatVec3(vertexIndex, vertex.v))
                            assert(false);

                        Eigen::Vector4f position(vertex.x, vertex.y, vertex.z, 1.0f);
                        Eigen::Vector4f transformedPosition = nodeMatrix * position;
                        vertex.x = transformedPosition.x();
                        vertex.y = transformedPosition.y();
                        vertex.z = transformedPosition.z();
                        RhcToLhcCoordinates(vertex.v);

                        if (vertexIndex == 0 && primIdx == 0)
                        {
                            xmodel.mins = vertex;
                            xmodel.maxs = vertex;
                        }
                        else
                            BSPUtil::updateAABBWithPoint(vertex, xmodel.mins, xmodel.maxs);
                    }
                }
            }
            else
            {
                xmodel.areBoundsValid = false;
                xmodel.mins.x = 0.0f;
                xmodel.mins.y = 0.0f;
                xmodel.mins.z = 0.0f;
                xmodel.maxs.x = 0.0f;
                xmodel.maxs.y = 0.0f;
                xmodel.maxs.z = 0.0f;
            }
        }

        bool addXModelNode(const JsonRoot& jRoot, const gltf::JsonNode& node)
        {
            assert(node.extras);
            assert(node.extras->contains("xmodel"));

            Eigen::Matrix4f nodeMatrix = createNodeMatrix(node);
            BSPXModel xmodel;

            xmodel.name = node.extras->at("xmodel");

            xmodel.doesCastShadow = true;
            if (node.extras->contains("flags"))
            {
                std::vector<std::string> flagStrVec = utils::StringSplit(node.extras->at("flags"), ',');
                for (std::string& flag : flagStrVec)
                    if (!flag.compare(surfaceTypeToNameMap[SURF_TYPE_NOCASTSHADOW]))
                    {
                        xmodel.doesCastShadow = false;
                        break;
                    }
            }

            Eigen::Vector4f position(0, 0, 0, 1.0f);
            Eigen::Vector4f transformedPosition = nodeMatrix * position;
            xmodel.origin.x = transformedPosition.x();
            xmodel.origin.y = transformedPosition.y();
            xmodel.origin.z = transformedPosition.z();
            RhcToLhcCoordinates(xmodel.origin.v);

            Eigen::Affine3f affineTransform(nodeMatrix);
            Eigen::Quaternionf rotationQuat(affineTransform.rotation());
            rotationQuat.normalize();
            xmodel.rotationQuaternion.x = rotationQuat.x();
            xmodel.rotationQuaternion.y = rotationQuat.y();
            xmodel.rotationQuaternion.z = rotationQuat.z();
            xmodel.rotationQuaternion.w = rotationQuat.w();
            RhcToLhcQuaternion(xmodel.rotationQuaternion.v);

            con::warn("XModels don't support scale currently, keep it at 1 in your editor");
            xmodel.scale = 1.0f;

            calculateXmodelBounds(xmodel, node.mesh, nodeMatrix, jRoot);

            m_curr_bsp_world->xmodels.emplace_back(xmodel);

            return true;
        }

        bool addSpawnPointNode(const gltf::JsonNode& node)
        {
            assert(node.extras);
            assert(node.extras->contains("spawnpoint"));

            Eigen::Matrix4f nodeMatrix = createNodeMatrix(node);

            Eigen::Vector4f position(0, 0, 0, 1.0f);
            Eigen::Vector4f transformedPosition = nodeMatrix * position;
            vec3_t origin;
            origin.x = transformedPosition.x();
            origin.y = transformedPosition.y();
            origin.z = transformedPosition.z();
            RhcToLhcCoordinates(origin.v);

            // GLTF default direction is +Y up
            Eigen::Vector3f defaultDirection(0.0f, 1.0f, 0.0f);
            Eigen::Affine3f affineTransform(nodeMatrix);
            Eigen::Matrix3f rotationMatrix = affineTransform.rotation();
            Eigen::Vector3f outputDirection = rotationMatrix * defaultDirection;
            outputDirection.normalize();
            vec3_t forward;
            forward.x = outputDirection.x();
            forward.y = outputDirection.y();
            forward.z = outputDirection.z();
            RhcToLhcCoordinates(forward.v);

            std::string team = node.extras->at("spawnpoint");
            if (!m_bsp->isZombiesMap && team.compare("attacker") && team.compare("defender") && team.compare("all"))
            {
                con::warn("Ignoring spawn point with an invalid type (must be attacker, defender or all)");
                return false;
            }

            BSPSpawnPoint spawnPoint;
            spawnPoint.origin = origin;
            spawnPoint.forward = forward;
            spawnPoint.spawnpointGroupName = team;
            m_bsp->spawnpoints.emplace_back(spawnPoint);

            return true;
        }

        size_t addScriptBrushModel(const JsonRoot& jRoot, const gltf::JsonNode& node)
        {
            if (!node.mesh || !jRoot.meshes)
                throw new GltfLoadException("Script model created with no mesh data");

            Eigen::Matrix4f nodeMatrix = createNodeMatrix(node);
            const auto& mesh = jRoot.meshes.value()[node.mesh.value()];

            if (mesh.primitives.size() == 0)
                throw new GltfLoadException("Script model created with no mesh data");

            BSPModel model;
            model.isGfxModel = m_is_world_gfx;
            model.surfaceIndex = 0;
            model.surfaceCount = 0;
            model.hasBrush = true;
            model.brushIndex = m_curr_bsp_world->scriptBoxBrushes.size();
            m_bsp->models.emplace_back(model);

            BSPBoxBrush boxBrush;
            boxBrush.contentFlags = 1;
            boxBrush.surfaceFlags = 1;
            vec3_t worldMins;
            vec3_t worldMaxs;
            for (size_t primIdx = 0; primIdx < mesh.primitives.size(); primIdx++)
            {
                const auto& primitive = mesh.primitives.at(primIdx);

                if (!primitive.attributes.POSITION)
                    throw GltfLoadException("Requires primitives attribute POSITION");

                // clang-format off
                 const auto* positionAccessor = GetAccessorForIndex(
                     "POSITION",
                     primitive.attributes.POSITION,
                     { JsonAccessorType::VEC3 },
                     { JsonAccessorComponentType::FLOAT }
                 ).value_or(nullptr);
                // clang-format on
                assert(positionAccessor != nullptr);

                if (positionAccessor->GetCount() == 0)
                    throw GltfLoadException("positionAccessor has count of 0");

                for (size_t vertexIndex = 0u; vertexIndex < positionAccessor->GetCount(); vertexIndex++)
                {
                    vec3_t vertex;
                    if (!positionAccessor->GetFloatVec3(vertexIndex, vertex.v))
                        assert(false);

                    Eigen::Vector4f position(vertex.x, vertex.y, vertex.z, 1.0f);
                    Eigen::Vector4f transformedPosition = nodeMatrix * position;
                    vertex.x = transformedPosition.x();
                    vertex.y = transformedPosition.y();
                    vertex.z = transformedPosition.z();
                    RhcToLhcCoordinates(vertex.v);

                    if (vertexIndex == 0 && primIdx == 0)
                    {
                        worldMins = vertex;
                        worldMaxs = vertex;
                    }
                    else
                        BSPUtil::updateAABBWithPoint(vertex, worldMins, worldMaxs);
                }
            }

            // convert world position to local position
            Eigen::Vector4f position(0, 0, 0, 1.0f);
            Eigen::Vector4f transformedPosition = nodeMatrix * position;
            vec3_t origin;
            origin.x = transformedPosition.x();
            origin.y = transformedPosition.y();
            origin.z = transformedPosition.z();
            RhcToLhcCoordinates(origin.v);
            boxBrush.localMins.x = worldMins.x - origin.x;
            boxBrush.localMins.y = worldMins.y - origin.y;
            boxBrush.localMins.z = worldMins.z - origin.z;
            boxBrush.localMaxs.x = worldMaxs.x - origin.x;
            boxBrush.localMaxs.y = worldMaxs.y - origin.y;
            boxBrush.localMaxs.z = worldMaxs.z - origin.z;

            m_curr_bsp_world->scriptBoxBrushes.emplace_back(boxBrush);

            return m_bsp->models.size(); // script model index starts at 1
        }

        size_t addScriptTerrainModel(const JsonRoot& jRoot, const gltf::JsonNode& node)
        {
            if (!node.mesh || !jRoot.meshes)
                throw new GltfLoadException("Script model created with no mesh data");

            Eigen::Matrix4f nodeMatrix = createNodeMatrix(node);
            const auto& mesh = jRoot.meshes.value()[node.mesh.value()];

            if (mesh.primitives.size() == 0)
                throw new GltfLoadException("Script model created with no mesh data");

            BSPModel model;
            model.isGfxModel = m_is_world_gfx;
            model.surfaceIndex = m_curr_bsp_world->scriptSurfaces.size();
            model.surfaceCount = mesh.primitives.size();
            model.hasBrush = false;
            model.brushIndex = 0;
            m_bsp->models.emplace_back(model);

            for (const auto& primitive : mesh.primitives)
            {
                if (!primitive.indices)
                    throw GltfLoadException("Requires primitives indices");
                if (primitive.mode.value_or(JsonMeshPrimitivesMode::TRIANGLES) != JsonMeshPrimitivesMode::TRIANGLES)
                    throw GltfLoadException("Only triangles are supported");
                if (!primitive.attributes.POSITION)
                    throw GltfLoadException("Requires primitives attribute POSITION");
                if (!primitive.attributes.NORMAL)
                    throw GltfLoadException("Requires primitives attribute NORMAL");

                const AccessorsForVertex accessorsForVertex{
                    .m_position_accessor = *primitive.attributes.POSITION,
                    .m_normal_accessor = *primitive.attributes.NORMAL,
                    .m_color_accessor = primitive.attributes.COLOR_0,
                    .m_uv_accessor = primitive.attributes.TEXCOORD_0,
                    .m_index_accessor = *primitive.indices,
                };

                size_t materialIndex;
                if (primitive.material)
                {
                    size_t originalMaterialIdx = *primitive.material;
                    if (node.extras && node.extras->contains("flags"))
                    {
                        bool isNoDrawFlagSet;
                        materialIndex = createMaterialWithFlags(originalMaterialIdx, node.extras->at("flags"), isNoDrawFlagSet);
                    }
                    else
                        materialIndex = originalMaterialIdx;
                }
                else
                    materialIndex = m_emptyMaterialIndex;

                BSPSurface surface;
                CreateSurface(accessorsForVertex, nodeMatrix, materialIndex, true, surface);
                m_curr_bsp_world->scriptSurfaces.emplace_back(surface);
            }

            return m_bsp->models.size(); // script model index starts at 1
        }

        bool addZoneNode(const JsonRoot& jRoot, const gltf::JsonNode& node)
        {
            assert(node.extras);
            assert(node.extras->contains("zone"));

            if (!node.extras->contains("spawner_group") || !node.extras->contains("spawnpoint_group"))
            {
                con::error("ignoring zone: Zone object must have a valid spawner_group and spawnpoint_group property");
                return false;
            }

            Eigen::Matrix4f nodeMatrix = createNodeMatrix(node);

            Eigen::Vector4f position(0, 0, 0, 1.0f);
            Eigen::Vector4f transformedPosition = nodeMatrix * position;
            vec3_t origin;
            origin.x = transformedPosition.x();
            origin.y = transformedPosition.y();
            origin.z = transformedPosition.z();
            RhcToLhcCoordinates(origin.v);

            BSPZoneZM zone;
            zone.origin = origin;
            zone.zoneName = node.extras->at("zone");
            zone.spawnerGroupName = node.extras->at("spawner_group");
            zone.spawnpointGroupName = node.extras->at("spawnpoint_group");
            zone.modelIndex = addScriptBrushModel(jRoot, node);
            m_bsp->zm_zones.emplace_back(zone);

            return true;
        }

        bool addZSpawnerNode(const gltf::JsonNode& node)
        {
            assert(node.extras);
            assert(node.extras->contains("spawner"));

            Eigen::Matrix4f nodeMatrix = createNodeMatrix(node);

            Eigen::Vector4f position(0, 0, 0, 1.0f);
            Eigen::Vector4f transformedPosition = nodeMatrix * position;
            vec3_t origin;
            origin.x = transformedPosition.x();
            origin.y = transformedPosition.y();
            origin.z = transformedPosition.z();
            RhcToLhcCoordinates(origin.v);

            // GLTF default direction is +Y up
            Eigen::Vector3f defaultDirection(0.0f, 1.0f, 0.0f);
            Eigen::Affine3f affineTransform(nodeMatrix);
            Eigen::Matrix3f rotationMatrix = affineTransform.rotation();
            Eigen::Vector3f outputDirection = rotationMatrix * defaultDirection;
            outputDirection.normalize();
            vec3_t forward;
            forward.x = outputDirection.x();
            forward.y = outputDirection.y();
            forward.z = outputDirection.z();
            RhcToLhcCoordinates(forward.v);

            BSPZSpawnerZM spawner;
            spawner.origin = origin;
            spawner.forward = forward;
            spawner.spawnerGroupName = node.extras->at("spawner");
            m_bsp->zm_spawners.emplace_back(spawner);

            return true;
        }

        bool addClassNode(const JsonRoot& jRoot, const gltf::JsonNode& node)
        {
            assert(node.extras);
            assert(node.extras->contains("classname"));

            BSPEntity entity;
            bool doesClassContainModel = false;
            for (auto& element : node.extras->items())
            {
                std::string key = element.key();
                if (!key.compare("origin") || !key.compare("angles") || !key.compare("flags"))
                    continue;

                if (!key.compare("model"))
                    doesClassContainModel = true;

                BSPEntityEntry entry;
                entry.key = key;
                entry.value = element.value();
                entity.entries.emplace_back(entry);
            }

            if (node.mesh && !doesClassContainModel)
            {
                if (m_is_world_gfx)
                    entity.modelIndex = addScriptTerrainModel(jRoot, node);
                else
                    entity.modelIndex = addScriptBrushModel(jRoot, node);
            }
            else
                entity.modelIndex = 0;

            Eigen::Matrix4f nodeMatrix = createNodeMatrix(node);

            Eigen::Vector4f position(0, 0, 0, 1.0f);
            Eigen::Vector4f transformedPosition = nodeMatrix * position;
            entity.origin.x = transformedPosition.x();
            entity.origin.y = transformedPosition.y();
            entity.origin.z = transformedPosition.z();
            RhcToLhcCoordinates(entity.origin.v);

            Eigen::Affine3f affineTransform(nodeMatrix);
            Eigen::Quaternionf rotationQuat(affineTransform.rotation());
            rotationQuat.normalize();
            entity.rotationQuaternion.x = rotationQuat.x();
            entity.rotationQuaternion.y = rotationQuat.y();
            entity.rotationQuaternion.z = rotationQuat.z();
            entity.rotationQuaternion.w = rotationQuat.w();
            RhcToLhcQuaternion(entity.rotationQuaternion.v);

            m_bsp->entities.emplace_back(entity);
            return true;
        }

        bool addNodeToBSP(const JsonRoot& jRoot, const gltf::JsonNode& node)
        {
            if (m_is_world_gfx && node.extensions && node.extensions->KHR_lights_punctual)
                return addLightNode(node);

            if (node.extras)
            {
                if (!m_is_world_gfx)
                {
                    if (node.extras->contains("classname"))
                        return addClassNode(jRoot, node);

                    if (node.extras->contains("spawnpoint"))
                        return addSpawnPointNode(node);

                    if (m_bsp->isZombiesMap)
                    {
                        if (node.extras->contains("zone"))
                            return addZoneNode(jRoot, node);

                        if (node.extras->contains("spawner"))
                            return addZSpawnerNode(node);
                    }
                }

                if (node.extras->contains("xmodel"))
                    return addXModelNode(jRoot, node);
            }

            if (node.mesh)
                return addMeshNode(jRoot, node);

            return false;
        }

        static std::vector<unsigned> GetRootNodes(const JsonRoot& jRoot)
        {
            if (!jRoot.nodes || jRoot.nodes->empty())
                return {};

            const auto nodeCount = jRoot.nodes->size();
            std::vector<unsigned> rootNodes;
            std::vector<bool> isChild(nodeCount);

            for (const auto& node : jRoot.nodes.value())
            {
                if (!node.children)
                    continue;

                for (const auto childIndex : node.children.value())
                {
                    if (childIndex >= nodeCount)
                        throw GltfLoadException("Illegal child index");

                    if (isChild[childIndex])
                        throw GltfLoadException("Node hierarchy is not a set of disjoint strict trees");

                    isChild[childIndex] = true;
                }
            }

            for (auto nodeIndex = 0u; nodeIndex < nodeCount; nodeIndex++)
            {
                if (!isChild[nodeIndex])
                    rootNodes.emplace_back(nodeIndex);
            }

            return rootNodes;
        }

        void LoadMaterials(const JsonRoot& jRoot)
        {
            if (jRoot.materials)
            {
                m_curr_bsp_world->materials.reserve((*jRoot.materials).size());
                for (auto& jsMaterial : *jRoot.materials)
                {
                    BSPMaterial material;

                    if (jsMaterial.name && (*jsMaterial.name).length() != 0)
                        material.materialName = *jsMaterial.name;
                    else
                        throw GltfLoadException("Materials must have a name.");

                    if (jsMaterial.pbrMetallicRoughness)
                    {
                        if (jsMaterial.pbrMetallicRoughness->baseColorFactor)
                        {
                            material.materialColour.x = (*jsMaterial.pbrMetallicRoughness->baseColorFactor)[0];
                            material.materialColour.y = (*jsMaterial.pbrMetallicRoughness->baseColorFactor)[1];
                            material.materialColour.z = (*jsMaterial.pbrMetallicRoughness->baseColorFactor)[2];
                            material.materialColour.w = (*jsMaterial.pbrMetallicRoughness->baseColorFactor)[3];
                        }
                        else
                        {

                            material.materialColour.x = 1.0f;
                            material.materialColour.y = 1.0f;
                            material.materialColour.z = 1.0f;
                            material.materialColour.w = 1.0f;
                        }

                        if (jsMaterial.pbrMetallicRoughness->baseColorTexture)
                            material.materialType = MATERIAL_TYPE_TEXTURE;
                        else
                            material.materialType = MATERIAL_TYPE_COLOUR;
                    }
                    else
                    {
                        material.materialType = MATERIAL_TYPE_COLOUR;
                        material.materialColour.x = 1.0f;
                        material.materialColour.y = 1.0f;
                        material.materialColour.z = 1.0f;
                        material.materialColour.w = 1.0f;
                    }

                    material.surfaceFlags = 0;
                    material.contentFlags = 1;
                    if (jsMaterial.extras && jsMaterial.extras->type)
                    {
                        bool foundType = false;
                        size_t typeNameCount = std::extent<decltype(BSPFlags::materialTypes)>::value;
                        for (size_t typeIdx = 0; typeIdx < typeNameCount; typeIdx++)
                        {
                            BSPFlags::SurfaceType surfType = BSPFlags::materialTypes[typeIdx];
                            if (!jsMaterial.extras->type->compare(BSPFlags::surfaceTypeToNameMap[surfType]))
                            {
                                BSPFlags::s_SurfaceTypeFlags flags = BSPFlags::surfaceTypeToFlagMap[surfType];
                                material.surfaceFlags |= flags.surfaceFlags;
                                material.contentFlags |= flags.contentFlags;
                                foundType = true;
                                break;
                            }
                        }
                        if (!foundType)
                            con::warn("material {} with invalid type name: {}", material.materialName, *jsMaterial.extras->type);
                    }

                    m_curr_bsp_world->materials.emplace_back(material);
                }
            }

            m_emptyMaterialIndex = m_curr_bsp_world->materials.size();
            BSPMaterial emptyMaterial;
            emptyMaterial.materialType = MATERIAL_TYPE_COLOUR;
            emptyMaterial.surfaceFlags = 0;
            emptyMaterial.contentFlags = 1;
            emptyMaterial.materialName = "";
            emptyMaterial.materialColour.x = 1.0f;
            emptyMaterial.materialColour.y = 1.0f;
            emptyMaterial.materialColour.z = 1.0f;
            emptyMaterial.materialColour.w = 1.0f;
            m_curr_bsp_world->materials.emplace_back(emptyMaterial);
        }

        void LoadLights(const JsonRoot& jRoot)
        {
            if (!m_is_world_gfx)
                return;
            if (!jRoot.extensions)
                return;
            if (!jRoot.extensions->KHR_lights_punctual)
                return;
            if (!jRoot.extensions->KHR_lights_punctual->lights)
                return;

            const std::vector<JsonPunctualLight>& jsLightArray = jRoot.extensions->KHR_lights_punctual->lights.value();
            m_bsp->lights.reserve(jsLightArray.size());
            for (const JsonPunctualLight& jsLight : jsLightArray)
            {
                if (jsLight.type == JsonPunctualLightType::POINT)
                    con::error("Any point lights will be converted to a spotlight as point lights are unsupported right now.");

                BSPLight light{};

                // position and direction data will be set during node traversal
                light.hasPosBeenSet = false;

                if (!jsLight.color)
                {
                    light.colour.x = 1.0f;
                    light.colour.y = 1.0f;
                    light.colour.z = 1.0f;
                }
                else
                {
                    light.colour.x = (*jsLight.color)[0];
                    light.colour.y = (*jsLight.color)[1];
                    light.colour.z = (*jsLight.color)[2];
                }

                if (!jsLight.intensity)
                    light.intensity = 100000.0f; // adjusted from spec to better match BO2
                else
                    light.intensity = *jsLight.intensity;

                if (!jsLight.range)
                    light.range = 1000.0f; // adjusted from spec to better match BO2
                else
                    light.range = *jsLight.range;

                if (jsLight.type == JsonPunctualLightType::DIRECTIONAL)
                {
                    light.type = LIGHT_TYPE_DIRECTIONAL;
                }
                else if (jsLight.type == JsonPunctualLightType::POINT)
                {
                    light.type = LIGHT_TYPE_POINT;
                }
                else // JsonPunctualLightType::SPOT
                {
                    light.type = LIGHT_TYPE_SPOT;

                    assert(jsLight.spot);
                    if (!jsLight.spot->innerConeAngle)
                        light.innerConeAngle = 0.0f;
                    else
                        light.innerConeAngle = *jsLight.spot->innerConeAngle;

                    if (!jsLight.spot->outerConeAngle)
                        light.outerConeAngle = 3.14159265359f / 4.0f; /// spec of 45 degrees
                    else
                        light.outerConeAngle = *jsLight.spot->outerConeAngle;
                }

                m_bsp->lights.emplace_back(light);
            }
        }

        void TraverseNodes(const JsonRoot& jRoot)
        {
            // Make sure there are any nodes to traverse
            if (!jRoot.nodes || jRoot.nodes->empty())
                return;

            std::deque<unsigned> nodeQueue;
            const std::vector<unsigned> rootNodes = GetRootNodes(jRoot);

            for (const auto rootNode : rootNodes)
                nodeQueue.emplace_back(rootNode);

            while (!nodeQueue.empty())
            {
                const auto& node = jRoot.nodes.value()[nodeQueue.front()];
                nodeQueue.pop_front();

                if (node.children)
                {
                    for (const auto childIndex : *node.children)
                        nodeQueue.emplace_back(childIndex);

                    if (node.matrix || node.translation || node.rotation || node.scale)
                        con::warn("Parent node has position data that won't be used");
                }

                if (!addNodeToBSP(jRoot, node))
                    con::warn("({}) Ignoring node: {}", getWorldTypeName(), node.name.value_or("unnamed node"));
            }
        }

        void CreateBuffers(const JsonRoot& jRoot, Input& gltfInput)
        {
            if (!jRoot.buffers)
                return;

            m_buffers.reserve(jRoot.buffers->size());
            for (const auto& jBuffer : *jRoot.buffers)
            {
                if (!jBuffer.uri)
                {
                    const void* embeddedBufferPtr = nullptr;
                    size_t embeddedBufferSize = 0u;
                    if (!gltfInput.GetEmbeddedBuffer(embeddedBufferPtr, embeddedBufferSize) || embeddedBufferSize == 0u)
                        throw GltfLoadException("Buffer tried to access embedded data when there is none");

                    m_buffers.emplace_back(std::make_unique<EmbeddedBuffer>(embeddedBufferPtr, embeddedBufferSize));
                }
                else if (DataUriBuffer::IsDataUri(*jBuffer.uri))
                {
                    auto dataUriBuffer = std::make_unique<DataUriBuffer>();
                    if (!dataUriBuffer->ReadDataFromUri(*jBuffer.uri))
                        throw GltfLoadException("Buffer has invalid data uri");

                    m_buffers.emplace_back(std::move(dataUriBuffer));
                }
                else
                {
                    throw GltfLoadException("File buffers are not supported");
                }
            }
        }

        void CreateBufferViews(const JsonRoot& jRoot)
        {
            if (!jRoot.bufferViews)
                return;

            m_buffer_views.reserve(jRoot.bufferViews->size());
            for (const auto& jBufferView : *jRoot.bufferViews)
            {
                if (jBufferView.buffer >= m_buffers.size())
                    throw GltfLoadException("Buffer view references invalid buffer");

                const auto* buffer = m_buffers[jBufferView.buffer].get();
                const auto offset = jBufferView.byteOffset.value_or(0u);
                const auto length = jBufferView.byteLength;
                const auto stride = jBufferView.byteStride.value_or(0u);

                if (offset + length > buffer->GetSize())
                    throw GltfLoadException("Buffer view is defined larger as underlying buffer");

                m_buffer_views.emplace_back(std::make_unique<BufferView>(buffer, offset, length, stride));
            }
        }

        void CreateAccessors(const JsonRoot& jRoot)
        {
            if (!jRoot.accessors)
                return;

            m_accessors.reserve(jRoot.accessors->size());
            for (const auto& jAccessor : *jRoot.accessors)
            {
                if (!jAccessor.bufferView)
                {
                    m_accessors.emplace_back(std::make_unique<NullAccessor>(jAccessor.count));
                    continue;
                }

                if (*jAccessor.bufferView >= m_buffer_views.size())
                    throw GltfLoadException("Accessor references invalid buffer view");

                const auto* bufferView = m_buffer_views[*jAccessor.bufferView].get();
                const auto byteOffset = jAccessor.byteOffset.value_or(0u);
                if (jAccessor.componentType == JsonAccessorComponentType::FLOAT)
                    m_accessors.emplace_back(std::make_unique<FloatAccessor>(bufferView, jAccessor.type, byteOffset, jAccessor.count));
                else if (jAccessor.componentType == JsonAccessorComponentType::UNSIGNED_BYTE)
                    m_accessors.emplace_back(std::make_unique<UnsignedByteAccessor>(bufferView, jAccessor.type, byteOffset, jAccessor.count));
                else if (jAccessor.componentType == JsonAccessorComponentType::UNSIGNED_SHORT)
                    m_accessors.emplace_back(std::make_unique<UnsignedShortAccessor>(bufferView, jAccessor.type, byteOffset, jAccessor.count));
                else if (jAccessor.componentType == JsonAccessorComponentType::UNSIGNED_INT)
                    m_accessors.emplace_back(std::make_unique<UnsignedIntAccessor>(bufferView, jAccessor.type, byteOffset, jAccessor.count));
                else
                    throw GltfLoadException(std::format("Accessor has unsupported component type {}", static_cast<unsigned>(jAccessor.componentType)));
            }
        }

    public:
        bool addGLTFDataToBSP(Input& gltfInput, bool isGfxWorld)
        {
            JsonRoot jRoot;
            try
            {
                jRoot = gltfInput.GetJson().get<JsonRoot>();
            }
            catch (const nlohmann::json::exception& e)
            {
                con::error("Failed to parse GLTF JSON: {}", e.what());
                return false;
            }

            try
            {
                m_is_world_gfx = isGfxWorld;
                if (isGfxWorld)
                    m_curr_bsp_world = &m_bsp->gfxWorld;
                else
                    m_curr_bsp_world = &m_bsp->colWorld;
                m_accessors.clear();
                m_buffer_views.clear();
                m_buffers.clear();

                CreateBuffers(jRoot, gltfInput);
                CreateBufferViews(jRoot);
                CreateAccessors(jRoot);

                if (isGfxWorld) // lights aren't needed in collision data
                    LoadLights(jRoot);

                LoadMaterials(jRoot);
                TraverseNodes(jRoot); // requires materials and lights
            }
            catch (const GltfLoadException& e)
            {
                con::error("Failed to load GLTF: {}", e.Str());
                return false;
            }

            return true;
        }

        BSPLoader(BSPData* bsp)
            : m_bsp(bsp)
        {
            m_curr_bsp_world = nullptr;
            m_is_world_gfx = false;
        };
    };
} // namespace

namespace BSP
{
    std::unique_ptr<BSPData> createBSPData(std::string& mapName, ISearchPath& searchPath, bool isZombiesMap)
    {
        bool seperateColFile = true;
        bool isGfxFileGltf = true;
        bool isColFileGltf = true;

        std::string gfxFilePath = BSPUtil::getFileNameForBSPAsset("map_gfx.gltf");
        auto gfxFile = searchPath.Open(gfxFilePath);
        if (!gfxFile.IsOpen())
        {
            isGfxFileGltf = false;
            gfxFilePath = BSPUtil::getFileNameForBSPAsset("map_gfx.glb");
            gfxFile = searchPath.Open(gfxFilePath);
            if (!gfxFile.IsOpen())
            {
                con::error("BSP Creator: Can't find map_gfx.gltf or map_gfx.glb.");
                return nullptr;
            }
        }

        std::string colFilePath = BSPUtil::getFileNameForBSPAsset("map_col.gltf");
        auto colFile = searchPath.Open(colFilePath);
        if (!colFile.IsOpen())
        {
            isColFileGltf = false;
            colFilePath = BSPUtil::getFileNameForBSPAsset("map_col.glb");
            colFile = searchPath.Open(colFilePath);
            if (!colFile.IsOpen())
            {
                con::info("BSP Creator: generating colision data from GLTF graphics data.");
                seperateColFile = false;
            }
        }

        std::unique_ptr<BSPData> bsp = std::make_unique<BSPData>();
        bsp->name = mapName;
        bsp->bspName = "maps/mp/" + mapName + ".d3dbsp";
        bsp->isZombiesMap = isZombiesMap;

        BSPLoader loader(bsp.get());
        if (isGfxFileGltf)
        {
            gltf::TextInput input;
            if (!input.ReadGltfData(*gfxFile.m_stream))
                return nullptr;
            if (!loader.addGLTFDataToBSP(input, true))
                return nullptr;
            if (!seperateColFile)
                if (!loader.addGLTFDataToBSP(input, false))
                    return nullptr;
        }
        else
        {
            gltf::BinInput input;
            if (!input.ReadGltfData(*gfxFile.m_stream))
                return nullptr;
            if (!loader.addGLTFDataToBSP(input, true))
                return nullptr;
            if (!seperateColFile)
                if (!loader.addGLTFDataToBSP(input, false))
                    return nullptr;
        }

        if (seperateColFile)
        {
            if (isColFileGltf)
            {
                gltf::TextInput input;
                if (!input.ReadGltfData(*colFile.m_stream))
                    return nullptr;
                if (!loader.addGLTFDataToBSP(input, false))
                    return nullptr;
            }
            else
            {
                gltf::BinInput input;
                if (!input.ReadGltfData(*colFile.m_stream))
                    return nullptr;
                if (!loader.addGLTFDataToBSP(input, false))
                    return nullptr;
            }
        }

        return bsp;
    }
} // namespace BSP
