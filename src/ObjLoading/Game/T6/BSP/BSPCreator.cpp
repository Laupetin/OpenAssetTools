#include "BSPCreator.h"

#include "BSPUtil.h"
#include "XModel/Gltf/GltfBinInput.h"
#include "XModel/Gltf/GltfTextInput.h"
#include "XModel/Gltf/Internal/GltfAccessor.h"
#include "XModel/Gltf/Internal/GltfBuffer.h"
#include "XModel/Gltf/Internal/GltfBufferView.h"
#include "XModel/Gltf/JsonGltf.h"

#include <deque>
#include <exception>
#include <format>
#include <iostream>
#include <limits>
#include <numbers>
#include <string>

namespace
{
    struct AccessorsForVertex
    {
        unsigned m_position_accessor;
        unsigned m_normal_accessor;
        std::optional<unsigned> m_color_accessor;
        unsigned m_uv_accessor;
        unsigned m_index_accessor;
    };

    void RhcToLhcCoordinates(float (&coords)[3])
    {
        const float two[3]{coords[0], coords[1], coords[2]};

        coords[0] = two[0];
        coords[1] = -two[2];
        coords[2] = two[1];
    }

    void RhcToLhcScale(float (&coords)[3])
    {
        const float two[3]{coords[0], coords[1], coords[2]};

        coords[0] = two[0];
        coords[1] = two[2];
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
        const Input& m_input;
        BSPData* m_bsp;
        std::vector<std::unique_ptr<Accessor>> m_accessors;
        std::vector<std::unique_ptr<BufferView>> m_buffer_views;
        std::vector<std::unique_ptr<Buffer>> m_buffers;

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

        unsigned CreateVertices(const AccessorsForVertex& accessorsForVertex, BSPSurface& surface)
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
            ).value_or(nullptr);
            VerifyAccessorVertexCount("TEXCOORD_0", uvAccessor, vertexCount);
            assert(uvAccessor != nullptr);

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

            surface.triCount = faceCount;
            surface.indexOfFirstIndex = static_cast<int>(m_bsp->gfxWorld.indices.size());
            surface.indexOfFirstVertex = static_cast<int>(m_bsp->gfxWorld.vertices.size());

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
                m_bsp->gfxWorld.indices.emplace_back(indices[0]);
                m_bsp->gfxWorld.indices.emplace_back(indices[1]);
                m_bsp->gfxWorld.indices.emplace_back(indices[2]);
            }

            const auto vertexOffset = static_cast<unsigned>(m_bsp->gfxWorld.vertices.size());
            m_bsp->gfxWorld.vertices.reserve(vertexOffset + vertexCount);
            for (auto vertexIndex = 0u; vertexIndex < vertexCount; vertexIndex++)
            {
                BSPVertex vertex;

                if (!positionAccessor->GetFloatVec3(vertexIndex, vertex.pos.v) || !normalAccessor->GetFloatVec3(vertexIndex, vertex.normal.v)
                    || !colorAccessor->GetFloatVec4(vertexIndex, vertex.color.v) || !uvAccessor->GetFloatVec2(vertexIndex, vertex.texCoord.v))
                {
                    assert(false);
                }

                RhcToLhcCoordinates(vertex.pos.v);
                RhcToLhcCoordinates(vertex.normal.v);

                m_bsp->gfxWorld.vertices.emplace_back(vertex);
            }

            return vertexOffset;
        }

        void loadSurfaceMaterialData(const JsonRoot& jRoot, const JsonMeshPrimitives& primitive, BSPSurface& surface)
        {
            BSPMaterialType matType;
            if (!primitive.material)
                if (!primitive.attributes.COLOR_0)
                {
                    // matType = MATERIAL_TYPE_EMPTY;
                    throw GltfLoadException("Primitive requires material or colour data.");
                }
                else
                    matType = MATERIAL_TYPE_COLOUR;
            else
                matType = MATERIAL_TYPE_TEXTURE;
            surface.material.materialType = matType;

            if (matType == MATERIAL_TYPE_TEXTURE)
            {
                if (!jRoot.materials || *primitive.material >= jRoot.materials->size())
                    throw GltfLoadException("Invalid material index");
                surface.material.materialName = jRoot.materials.value()[primitive.material.value()].name.value();
            }
            else
                surface.material.materialName = "";
        }

        bool CreateSurfaceFromPrimitive(const JsonRoot& jRoot, const JsonMeshPrimitives& primitive)
        {
            if (!primitive.indices)
                throw GltfLoadException("Requires primitives indices");
            if (primitive.mode.value_or(JsonMeshPrimitivesMode::TRIANGLES) != JsonMeshPrimitivesMode::TRIANGLES)
                throw GltfLoadException("Only triangles are supported");
            if (!primitive.attributes.POSITION)
                throw GltfLoadException("Requires primitives attribute POSITION");
            if (!primitive.attributes.NORMAL)
                throw GltfLoadException("Requires primitives attribute NORMAL");
            if (!primitive.attributes.TEXCOORD_0)
                throw GltfLoadException("Requires primitives attribute TEXCOORD_0");

            const AccessorsForVertex accessorsForVertex{
                .m_position_accessor = *primitive.attributes.POSITION,
                .m_normal_accessor = *primitive.attributes.NORMAL,
                .m_color_accessor = primitive.attributes.COLOR_0,
                .m_uv_accessor = *primitive.attributes.TEXCOORD_0,
                .m_index_accessor = *primitive.indices,
            };

            BSPSurface surface;

            loadSurfaceMaterialData(jRoot, primitive, surface);
            CreateVertices(accessorsForVertex, surface);

            m_bsp->gfxWorld.surfaces.emplace_back(surface);

            return true;
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
                }

                if (node.mesh)
                {
                    con::info("Mesh {} found", node.name.has_value() ? node.name.value() : "");

                    const auto& mesh = jRoot.meshes.value()[node.mesh.value()];
                    for (const auto& primitive : mesh.primitives)
                    {
                        CreateSurfaceFromPrimitive(jRoot, primitive);
                    }
                }
            }
        }

        void CreateBuffers(const JsonRoot& jRoot)
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
                    if (!m_input.GetEmbeddedBuffer(embeddedBufferPtr, embeddedBufferSize) || embeddedBufferSize == 0u)
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
        bool addGLTFDataToBSP(bool isGfxWorld)
        {
            JsonRoot jRoot;
            try
            {
                jRoot = m_input.GetJson().get<JsonRoot>();
            }
            catch (const nlohmann::json::exception& e)
            {
                con::error("Failed to parse GLTF JSON: {}", e.what());
                return false;
            }

            try
            {
                CreateBuffers(jRoot);
                CreateBufferViews(jRoot);
                CreateAccessors(jRoot);

                TraverseNodes(jRoot);
            }
            catch (const GltfLoadException& e)
            {
                con::error("Failed to load GLTF: {}", e.Str());
                return false;
            }
        }

        BSPLoader(const Input& input, BSPData* bsp)
            : m_input(input),
              m_bsp(bsp) {};
    };
} // namespace

namespace BSP
{
    std::unique_ptr<BSPData> createBSPData(std::string& mapName, ISearchPath& searchPath)
    {
        bool isGfxFileGltf = true;
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

        std::unique_ptr<BSPData> bsp = std::make_unique<BSPData>();

        bsp->name = mapName;
        bsp->bspName = "maps/mp/" + mapName + ".d3dbsp";

        if (isGfxFileGltf)
        {
            gltf::TextInput input;
            if (!input.ReadGltfData(*gfxFile.m_stream))
                return nullptr;

            BSPLoader loader(input, bsp.get());
            if (!loader.addGLTFDataToBSP(true))
                return nullptr;
        }
        else
        {
            gltf::BinInput input;
            if (!input.ReadGltfData(*gfxFile.m_stream))
                return nullptr;

            BSPLoader loader(input, bsp.get());
            if (!loader.addGLTFDataToBSP(true))
                return nullptr;
        }

        return bsp;
    }
} // namespace BSP

/*
std::unique_ptr<BSPData> createBSPData(std::string& mapName, ISearchPath& searchPath)
    {
        std::string gfxFbxFileName = "map_gfx.fbx";
        std::string gfxFbxPath = BSPUtil::getFileNameForBSPAsset(gfxFbxFileName);
        auto gfxFile = searchPath.Open(gfxFbxPath);
        if (!gfxFile.IsOpen())
        {
            con::error("Failed to open map gfx fbx file: {}", gfxFbxPath);
            return nullptr;
        }

        std::unique_ptr<char> gfxMapData(new char[static_cast<size_t>(gfxFile.m_length)]);
        gfxFile.m_stream->read(gfxMapData.get(), gfxFile.m_length);
        if (gfxFile.m_stream->gcount() != gfxFile.m_length)
        {
            con::error("Read error of gfx fbx file: {}", gfxFbxPath);
            return nullptr;
        }

        ufbx_error errorGfx;
        ufbx_load_opts optsGfx{};
        optsGfx.target_axes = ufbx_axes_right_handed_y_up;
        optsGfx.generate_missing_normals = true;
        optsGfx.allow_missing_vertex_position = false;
        ufbx_scene* gfxScene = ufbx_load_memory(gfxMapData.get(), static_cast<size_t>(gfxFile.m_length), &optsGfx, &errorGfx);
        if (!gfxScene)
        {
            con::error("Failed to load map gfx fbx file: {}", errorGfx.description.data);
            return nullptr;
        }

        ufbx_scene* colScene;
        std::string colFbxFileName = "map_col.fbx";
        std::string colFbxPath = BSPUtil::getFileNameForBSPAsset(colFbxFileName);
        auto colFile = searchPath.Open(colFbxPath);
        if (!colFile.IsOpen())
        {
            con::warn("Failed to open map collison fbx file: {}. map gfx will be used for collision instead.", colFbxPath);
            colScene = gfxScene;
        }
        else
        {
            std::unique_ptr<char> colMapData(new char[static_cast<size_t>(colFile.m_length)]);
            colFile.m_stream->seekg(0);
            colFile.m_stream->read(colMapData.get(), colFile.m_length);
            if (colFile.m_stream->gcount() != colFile.m_length)
            {
                con::error("Read error of collision fbx file: {}", colFbxPath);
                return nullptr;
            }

            ufbx_error errorCol;
            ufbx_load_opts optsCol{};
            optsCol.target_axes = ufbx_axes_right_handed_y_up;
            optsCol.generate_missing_normals = true;
            optsCol.allow_missing_vertex_position = false;
            colScene = ufbx_load_memory(colMapData.get(), static_cast<size_t>(colFile.m_length), &optsCol, &errorCol);
            if (!colScene)
            {
                con::error("Failed to load map collision fbx file: {}", errorCol.description.data);
                return nullptr;
            }
        }

        std::unique_ptr<BSPData> bsp = std::make_unique<BSPData>();

        bsp->name = mapName;
        bsp->bspName = "maps/mp/" + mapName + ".d3dbsp";

        loadWorldData(gfxScene, bsp.get(), true);
        loadWorldData(colScene, bsp.get(), false);

        ufbx_free_scene(gfxScene);
        if (gfxScene != colScene)
            ufbx_free_scene(colScene);

        return bsp;
    }











        using namespace BSP;

    void addFBXMeshToWorld(
        ufbx_node* node, std::vector<BSPSurface>& surfaceVec, std::vector<BSPVertex>& vertexVec, std::vector<uint16_t>& indexVec, bool& hasTangentSpace)
    {
        ufbx_mesh* mesh = node->mesh;

        assert(node->attrib_type == UFBX_ELEMENT_MESH);

        if (mesh->instances.count != 1)
            con::warn("mesh {} has {} instances, only the 1st instace will be used.", node->name.data, mesh->instances.count);

        if (mesh->num_triangles == 0)
        {
            con::warn("ignoring mesh {}: triangle count is 0.", node->name.data);
            return;
        }

        if (mesh->num_indices % 3 != 0)
        {
            con::warn("ignoring mesh {}: it is not triangulated.", node->name.data);
            return;
        }

        for (size_t k = 0; k < mesh->num_indices; k++)
        {
            if (mesh->vertex_indices[k] > UINT16_MAX)
            {
                con::warn("ignoring mesh {}, it has more than {} indices.", node->name.data, UINT16_MAX);
                return;
            }
        }

        if (mesh->vertex_tangent.exists == false)
            hasTangentSpace = false;

        // Fix the target_unit_meters ufbx opt not working
        // UFBX stores the transform data in units that are 100x larger than what blender uses, so this converts them back
        ufbx_transform origTransform = node->local_transform;
        origTransform.translation.x /= 100.0f;
        origTransform.translation.y /= 100.0f;
        origTransform.translation.z /= 100.0f;
        origTransform.scale.x /= 100.0f;
        origTransform.scale.y /= 100.0f;
        origTransform.scale.z /= 100.0f;
        ufbx_matrix meshMatrix = ufbx_transform_to_matrix(&origTransform);

        for (ufbx_mesh_part& meshPart : mesh->material_parts)
        {
            if (meshPart.num_faces == 0)
                continue;

            BSPSurface surface;
            size_t partTriangleNum = meshPart.num_triangles;
            surface.triCount = static_cast<int>(partTriangleNum);
            surface.indexOfFirstVertex = static_cast<int>(vertexVec.size());
            surface.indexOfFirstIndex = static_cast<int>(indexVec.size());

            if (mesh->materials.count == 0)
            {
                surface.material.materialType = MATERIAL_TYPE_EMPTY;
                surface.material.materialName = "";
            }
            else
            {
                surface.material.materialType = MATERIAL_TYPE_TEXTURE;
                surface.material.materialName = mesh->materials.data[meshPart.index]->name.data;
            }

            std::vector<BSPVertex> tempVertices;
            std::vector<uint32_t> tempIndices;
            tempIndices.resize(mesh->max_face_triangles * 3);
            for (uint32_t faceIndex : meshPart.face_indices)
            {
                ufbx_face* face = &mesh->faces.data[faceIndex];

                // Triangulate the face into the indices vector
                uint32_t triangluatedTriCount = ufbx_triangulate_face(tempIndices.data(), tempIndices.size(), mesh, *face);

                for (uint32_t idxOfIndex = 0; idxOfIndex < triangluatedTriCount * 3; idxOfIndex++)
                {
                    BSPVertex vertex;
                    uint32_t index = tempIndices[idxOfIndex];

                    ufbx_vec3 transformedPos = ufbx_transform_position(&meshMatrix, ufbx_get_vertex_vec3(&mesh->vertex_position, index));
                    vec3_t blenderCoords;
                    blenderCoords.x = static_cast<float>(transformedPos.x);
                    blenderCoords.y = static_cast<float>(transformedPos.y);
                    blenderCoords.z = static_cast<float>(transformedPos.z);
                    vertex.pos = BSPUtil::convertToBO2Coords(blenderCoords);

                    if (surface.material.materialType == MATERIAL_TYPE_TEXTURE || surface.material.materialType == MATERIAL_TYPE_EMPTY)
                    {
                        vertex.color.x = 1.0f;
                        vertex.color.y = 1.0f;
                        vertex.color.z = 1.0f;
                        vertex.color.w = 1.0f;
                    }
                    else // surface->material.materialType == MATERIAL_TYPE_COLOUR
                    {
                        float factor = static_cast<float>(mesh->materials.data[meshPart.index]->fbx.diffuse_factor.value_real);
                        ufbx_vec4 diffuse = mesh->materials.data[meshPart.index]->fbx.diffuse_color.value_vec4;
                        vertex.color.x = static_cast<float>(diffuse.x * factor);
                        vertex.color.y = static_cast<float>(diffuse.y * factor);
                        vertex.color.z = static_cast<float>(diffuse.z * factor);
                        vertex.color.w = static_cast<float>(diffuse.w * factor);
                    }

                    // 1.0f - uv.y reason: https://gamedev.stackexchange.com/questions/92886/fbx-uv-coordinates-is-strange
                    ufbx_vec2 uv = ufbx_get_vertex_vec2(&mesh->vertex_uv, index);
                    vertex.texCoord.x = static_cast<float>(uv.x);
                    vertex.texCoord.y = static_cast<float>(1.0f - uv.y);

                    ufbx_vec3 normal = ufbx_get_vertex_vec3(&mesh->vertex_normal, index);
                    vertex.normal.x = static_cast<float>(normal.x);
                    vertex.normal.y = static_cast<float>(normal.y);
                    vertex.normal.z = static_cast<float>(normal.z);

                    if (mesh->vertex_tangent.exists)
                    {
                        ufbx_vec3 tangent = ufbx_get_vertex_vec3(&mesh->vertex_tangent, index);
                        vertex.tangent.x = static_cast<float>(tangent.x);
                        vertex.tangent.y = static_cast<float>(tangent.y);
                        vertex.tangent.z = static_cast<float>(tangent.z);
                    }
                    else
                    {
                        vertex.tangent.x = 0.0f;
                        vertex.tangent.y = 0.0f;
                        vertex.tangent.z = 0.0f;
                    }

                    tempVertices.emplace_back(vertex);
                }
            }

            // Generate the index buffer.
            // ufbx_generate_indices will deduplicate vertices, modifying the arrays passed in streams,
            // indices are written to outIndices and the number of unique vertices is returned.
            ufbx_vertex_stream streams[1] = {
                {tempVertices.data(), tempVertices.size(), sizeof(BSPVertex)},
            };
            std::vector<uint32_t> outIndices;
            outIndices.resize(partTriangleNum * 3);
            size_t numGeneratedVertices = ufbx_generate_indices(streams, 1, outIndices.data(), outIndices.size(), nullptr, nullptr);
            assert(numGeneratedVertices != 0);

            // trim non-unique vertexes and add to the world vertex vector
            tempVertices.resize(numGeneratedVertices);
            vertexVec.insert(vertexVec.end(), tempVertices.begin(), tempVertices.end());

            // T6 uses unsigned shorts as their index type so we have to loop and convert them from an unsigned int
            for (size_t idx = 0; idx < outIndices.size(); idx += 3)
            {
                // BO2's index ordering is opposite to the FBX, so its converted here
                indexVec.emplace_back(static_cast<uint16_t>(outIndices[idx + 2]));
                indexVec.emplace_back(static_cast<uint16_t>(outIndices[idx + 1]));
                indexVec.emplace_back(static_cast<uint16_t>(outIndices[idx + 0]));
            }

            surfaceVec.emplace_back(surface);
        }
    }

    void loadWorldData(ufbx_scene* scene, BSPData* bsp, bool isGfxData)
    {
        bool hasTangentSpace = true;
        for (ufbx_node* node : scene->nodes)
        {
            if (node->attrib_type == UFBX_ELEMENT_MESH)
            {
                if (isGfxData)
                    addFBXMeshToWorld(node, bsp->gfxWorld.surfaces, bsp->gfxWorld.vertices, bsp->gfxWorld.indices, hasTangentSpace);
                else
                    addFBXMeshToWorld(node, bsp->colWorld.surfaces, bsp->colWorld.vertices, bsp->colWorld.indices, hasTangentSpace);
            }
            else
            {
                con::debug("ignoring node type {}: {}", static_cast<int>(node->attrib_type), node->name.data);
            }
        }

        if (hasTangentSpace == false)
            con::warn("warning: one or more meshes have no tangent space. Be sure to select the tangent space box when exporting the FBX.");
    }
*/
