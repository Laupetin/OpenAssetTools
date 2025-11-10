#include "BSPCreator.h"

#include "BSPUtil.h"

#include <format>
#include <ufbx.h>

using namespace BSP;

namespace
{
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
                surface.material.materialType = BSPMaterialType::MATERIAL_TYPE_EMPTY;
                surface.material.materialName = "";
            }
            else
            {
                surface.material.materialType = BSPMaterialType::MATERIAL_TYPE_TEXTURE;
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
                    T6::vec3_t blenderCoords;
                    blenderCoords.x = static_cast<float>(transformedPos.x);
                    blenderCoords.y = static_cast<float>(transformedPos.y);
                    blenderCoords.z = static_cast<float>(transformedPos.z);
                    vertex.pos = BSPUtil::convertToBO2Coords(blenderCoords);

                    if (surface.material.materialType == BSPMaterialType::MATERIAL_TYPE_TEXTURE
                        || surface.material.materialType == BSPMaterialType::MATERIAL_TYPE_EMPTY)
                    {
                        vertex.color.x = 1.0f;
                        vertex.color.y = 1.0f;
                        vertex.color.z = 1.0f;
                        vertex.color.w = 1.0f;
                    }
                    else
                    {
                        assert(surface.material.materialType == BSPMaterialType::MATERIAL_TYPE_COLOUR);

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

    void loadWorldData(const ufbx_scene& scene, BSPData& bsp, const bool isGfxData)
    {
        bool hasTangentSpace = true;
        for (ufbx_node* node : scene.nodes)
        {
            if (node->attrib_type == UFBX_ELEMENT_MESH)
            {
                if (isGfxData)
                    addFBXMeshToWorld(node, bsp.gfxWorld.surfaces, bsp.gfxWorld.vertices, bsp.gfxWorld.indices, hasTangentSpace);
                else
                    addFBXMeshToWorld(node, bsp.colWorld.surfaces, bsp.colWorld.vertices, bsp.colWorld.indices, hasTangentSpace);
            }
            else
            {
                con::debug("ignoring node type {}: {}", static_cast<int>(node->attrib_type), node->name.data);
            }
        }

        if (hasTangentSpace == false)
            con::warn("warning: one or more meshes have no tangent space. Be sure to select the tangent space box when exporting the FBX.");
    }
} // namespace

namespace BSP
{
    std::unique_ptr<BSPData> createBSPData(const std::string& mapName, ISearchPath& searchPath)
    {
        const auto gfxFbxPath = BSPUtil::getFileNameForBSPAsset("map_gfx.fbx");
        const auto gfxFile = searchPath.Open(gfxFbxPath);
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
        const auto colFbxPath = BSPUtil::getFileNameForBSPAsset("map_col.fbx");
        const auto colFile = searchPath.Open(colFbxPath);
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

        auto bsp = std::make_unique<BSPData>();

        bsp->name = mapName;
        bsp->bspName = std::format("maps/mp/{}.d3dbsp", mapName);

        loadWorldData(*gfxScene, *bsp, true);
        loadWorldData(*colScene, *bsp, false);

        ufbx_free_scene(gfxScene);
        if (gfxScene != colScene)
            ufbx_free_scene(colScene);

        return bsp;
    }
} // namespace BSP
