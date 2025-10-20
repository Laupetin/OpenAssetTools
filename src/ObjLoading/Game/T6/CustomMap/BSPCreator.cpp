#include "BSPCreator.h"

#include "fbx/ufbx.h"
#include "Game/T6/T6.h"
using namespace T6;

bool addFBXMeshToWorld(ufbx_node* node, 
    std::vector<CustomMapSurface>& surfaceVec,
    std::vector<CustomMapVertex>& vertexVec,
    std::vector<uint16_t>& indexVec, 
    bool& hasTangentSpace)
{
    if (node->attrib_type != UFBX_ELEMENT_MESH)
    {
        printf("ignoring non-mesh node \"%s\"\n", node->name.data);
        return false;
    }
    ufbx_mesh* mesh = node->mesh;

    if (mesh->instances.count != 1)
        printf("mesh %s has %i instances, only the 1st instace will be used.\n", node->name.data, mesh->instances.count);

    if (mesh->num_triangles == 0)
    {
        printf("ignoring mesh %s: triangle count is 0.\n", node->name.data);
        return false;
    }

    if (mesh->num_indices % 3 != 0)
    {
        printf("ignoring mesh %s: it is not triangulated.\n", node->name.data);
        return false;
    }

    for (size_t k = 0; k < mesh->num_indices; k++)
    {
        if (mesh->vertex_indices[k] > UINT16_MAX)
        {
            printf("ignoring mesh %s, it has more than %i indices.\n", node->name.data, UINT16_MAX);
            return false;
        }
    }

    if (mesh->vertex_tangent.exists == false)
        hasTangentSpace = false;

    
    // Fix the target_unit_meters opt not working 
    // UFBX stores the transform data in units that are 100x larger than what blender uses, so this converts them back
    ufbx_transform origTransform = node->local_transform;
    origTransform.translation.x /= 100.0f;
    origTransform.translation.y /= 100.0f;
    origTransform.translation.z /= 100.0f;
    origTransform.scale.x /= 100.0f;
    origTransform.scale.y /= 100.0f;
    origTransform.scale.z /= 100.0f;
    ufbx_matrix meshMatrix = ufbx_transform_to_matrix(&origTransform);

    // FBX loading code modified from https://ufbx.github.io/elements/meshes/
    // Seems like I have to use this exact way of loading, otherwise some values become incorrect
    for (size_t i = 0; i < mesh->material_parts.count; i++)
    {
        ufbx_mesh_part* meshPart = &mesh->material_parts.data[i];

        if (meshPart->num_faces == 0)
            continue;

        CustomMapSurface surface;
        surface.triCount = meshPart->num_triangles;
        surface.indexOfFirstVertex = vertexVec.size();
        surface.indexOfFirstIndex = indexVec.size();

        CustomMapMaterialType meshMaterialType;
        if (mesh->materials.count == 0)
        {
            meshMaterialType = MATERIAL_TYPE_EMPTY;
        }
        //else if (mesh->materials.data[i]->textures.count != 0)
        //{
        //    meshMaterialType = CM_MATERIAL_TEXTURE;
        //    surface.material.materialName = _strdup(mesh->materials.data[i]->name.data);
        //}
        //else
        //{
        //    meshMaterialType = CM_MATERIAL_COLOUR;
        //    surface.material.materialName = "";
        //}
        else
        {
            meshMaterialType = MATERIAL_TYPE_TEXTURE;
            surface.material.materialName = _strdup(mesh->materials.data[i]->name.data);
        }
        surface.material.materialType = meshMaterialType;

        size_t num_triangles = meshPart->num_triangles;
        CustomMapVertex* vertices = (CustomMapVertex*)calloc(num_triangles * 3, sizeof(CustomMapVertex));
        size_t num_vertices = 0;

        // Reserve space for the maximum triangle indices.
        size_t num_tri_indices = mesh->max_face_triangles * 3;
        uint32_t* tri_indices = (uint32_t*)calloc(num_tri_indices, sizeof(uint32_t));

        _ASSERT(meshPart->num_triangles == meshPart->num_faces);
        for (size_t face_ix = 0; face_ix < meshPart->num_faces; face_ix++)
        {
            ufbx_face face = mesh->faces.data[meshPart->face_indices.data[face_ix]];

            // Triangulate the face into `tri_indices[]`.
            uint32_t num_tris = ufbx_triangulate_face(tri_indices, num_tri_indices, mesh, face);

            // Iterate over each triangle corner contiguously.
            for (size_t q = 0; q < num_tris * 3; q++)
            {
                uint32_t index = tri_indices[q];

                CustomMapVertex* vertex = &vertices[num_vertices++];

                //ufbx_vec3 pos = ufbx_get_vertex_vec3(&mesh->vertex_position, index);
                //vertex->pos.x = static_cast<float>(pos.x);
                //vertex->pos.y = static_cast<float>(pos.y);
                //vertex->pos.z = static_cast<float>(pos.z);
                //  Fix the target_unit_meters opt not working 
                ufbx_vec3 transformedPos = ufbx_transform_position(&meshMatrix, ufbx_get_vertex_vec3(&mesh->vertex_position, index));
                vertex->pos.x = static_cast<float>(transformedPos.x);
                vertex->pos.y = static_cast<float>(transformedPos.y);
                vertex->pos.z = static_cast<float>(transformedPos.z);

                
                switch (meshMaterialType)
                {
                case MATERIAL_TYPE_TEXTURE:
                case MATERIAL_TYPE_EMPTY:
                    vertex->color.x = 1.0f;
                    vertex->color.y = 1.0f;
                    vertex->color.z = 1.0f;
                    vertex->color.w = 1.0f;
                    break;
                case MATERIAL_TYPE_COLOUR:
                {
                    float factor = static_cast<float>(mesh->materials.data[i]->fbx.diffuse_factor.value_real);
                    ufbx_vec4 diffuse = mesh->materials.data[i]->fbx.diffuse_color.value_vec4;
                    vertex->color.x = static_cast<float>(diffuse.x * factor);
                    vertex->color.y = static_cast<float>(diffuse.y * factor);
                    vertex->color.z = static_cast<float>(diffuse.z * factor);
                    vertex->color.w = static_cast<float>(diffuse.w * factor);
                    break;
                }

                default:
                    _ASSERT(false);
                }
                


                // 1.0f - uv.y reason:
                // https://gamedev.stackexchange.com/questions/92886/fbx-uv-coordinates-is-strange
                ufbx_vec2 uv = ufbx_get_vertex_vec2(&mesh->vertex_uv, index);
                vertex->texCoord.x = (float)(uv.x);
                vertex->texCoord.y = (float)(1.0f - uv.y);

                ufbx_vec3 normal = ufbx_get_vertex_vec3(&mesh->vertex_normal, index);
                vertex->normal.x = static_cast<float>(normal.x);
                vertex->normal.y = static_cast<float>(normal.y);
                vertex->normal.z = static_cast<float>(normal.z);

                if (mesh->vertex_tangent.exists)
                {
                    ufbx_vec3 tangent = ufbx_get_vertex_vec3(&mesh->vertex_tangent, index);
                    vertex->tangent.x = static_cast<float>(tangent.x);
                    vertex->tangent.y = static_cast<float>(tangent.y);
                    vertex->tangent.z = static_cast<float>(tangent.z);
                }
                else
                {
                    vertex->tangent.x = 0.0f;
                    vertex->tangent.y = 0.0f;
                    vertex->tangent.z = 0.0f;
                }
            }
        }

        _ASSERT(num_vertices == num_triangles * 3);

        // Generate the index buffer.
        ufbx_vertex_stream streams[1] = {
            {vertices, num_vertices, sizeof(CustomMapVertex)},
        };
        size_t num_indices = num_triangles * 3;
        uint32_t* indices = (uint32_t*)calloc(num_indices, sizeof(uint32_t));

        // This call will deduplicate vertices, modifying the arrays passed in `streams[]`,
        // indices are written in `indices[]` and the number of unique vertices is returned.
        num_vertices = ufbx_generate_indices(streams, 1, indices, num_indices, NULL, NULL);
        _ASSERT(num_vertices != 0);

        vertexVec.insert(vertexVec.end(), &vertices[0], &vertices[num_vertices]);

        size_t currIndexVecSize = indexVec.size();
        indexVec.resize(indexVec.size() + num_indices);
        for (size_t m = 0; m < num_indices; m++)
        {
            indexVec[currIndexVecSize + m] = (uint16_t)indices[m];
        }

        surfaceVec.push_back(surface);
    }

    return true;
}

void loadWorldData(ufbx_scene* scene, CustomMapBSP* bsp, bool isGfxData)
{
    bool hasTangentSpace = true;

    for (size_t i = 0; i < scene->nodes.count; i++)
    {
        ufbx_node* node = scene->nodes.data[i];

        if (node->attrib_type == UFBX_ELEMENT_MESH)
        {
            if (isGfxData)
                addFBXMeshToWorld(node, bsp->gfxWorld.surfaces, bsp->gfxWorld.vertices, bsp->gfxWorld.indices, hasTangentSpace);
            else
                addFBXMeshToWorld(node, bsp->colWorld.surfaces, bsp->colWorld.vertices, bsp->colWorld.indices, hasTangentSpace);
        }
        else
        {
            //printf("ignoring node type %i: %s\n", node->attrib_type, node->name.data);
        }
    }

    if (hasTangentSpace == false)
        printf("warning: one or more meshes have no tangent space. Be sure to select the tangent space box when exporting the FBX from blender.\n");
}

CustomMapBSP* BSPCreator::createCustomMapBSP(std::string& mapName, ISearchPath& searchPath)
{
    ufbx_scene* gfxScene;
    ufbx_scene* colScene;

    std::string gfxFbxPath = "custom_map/map_gfx.fbx";
    auto gfxFile = searchPath.Open(gfxFbxPath);
    if (!gfxFile.IsOpen())
    {
        printf("Failed to open map gfx fbx file: %s\n", gfxFbxPath.c_str());
        return NULL;
    }

    char* gfxMapData = new char[static_cast<unsigned int>(gfxFile.m_length)];
    gfxFile.m_stream->seekg(0);
    gfxFile.m_stream->read(gfxMapData, gfxFile.m_length);

    ufbx_error error;
    ufbx_load_opts opts; // IDK why but opts don't seem to be working correctly, target_unit_meters isn't being used
    memset(&opts, 0, sizeof(ufbx_load_opts));
    opts.target_axes = ufbx_axes_right_handed_y_up;
    opts.generate_missing_normals = true;
    opts.allow_missing_vertex_position = false;
    opts.target_unit_meters = 100.0f;
    //gfxScene = ufbx_load_memory(gfxMapData, static_cast<size_t>(gfxFile.m_length), &opts, &error);
    gfxScene = ufbx_load_memory(gfxMapData, static_cast<size_t>(gfxFile.m_length), NULL, &error);
    if (!gfxScene) 
    {
        fprintf(stderr, "Failed to load map gfx fbx file: %s\n", error.description.data);
        return NULL;
    }

    std::string colFbxPath = "custom_map/map_col.fbx";
    auto colFile = searchPath.Open(colFbxPath);
    if (!colFile.IsOpen())
    {
        printf("Failed to open map collison fbx file: %s. map gfx will be used for collision instead.\n", colFbxPath.c_str());
        colScene = gfxScene;
    }
    else
    {
        char* colMapData = new char[static_cast<unsigned int>(colFile.m_length)];
        colFile.m_stream->seekg(0);
        colFile.m_stream->read(colMapData, colFile.m_length);

        colScene = ufbx_load_memory(colMapData, static_cast<size_t>(colFile.m_length), &opts, &error);
        if (!colScene)
        {
            fprintf(stderr, "Failed to load map collision fbx file: %s\n", error.description.data);
            return NULL;
        }
    }

    CustomMapBSP* projInfo = new CustomMapBSP;

    projInfo->name = mapName;
    projInfo->bspName = "maps/mp/" + mapName + ".d3dbsp";

    loadWorldData(gfxScene, projInfo, true);
    loadWorldData(colScene, projInfo, false);

    ufbx_free_scene(gfxScene);
    if (gfxScene != colScene)
        ufbx_free_scene(colScene);

    return projInfo;
}
