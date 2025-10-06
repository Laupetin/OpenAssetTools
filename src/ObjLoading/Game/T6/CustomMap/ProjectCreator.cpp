#include "ProjectCreator.h"

#include "fbx/ufbx.h"

std::vector<customMapVertex> vertexVec;
std::vector<uint16_t> indexVec;
std::vector<worldSurface> surfaceVec;
std::vector<customMapModel> modelVec;
bool hasTangentSpace = true;

bool loadFBXMesh(ufbx_node* node)
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

        worldSurface surface;
        surface.triCount = meshPart->num_triangles;
        surface.firstVertexIndex = vertexVec.size();
        surface.firstIndex_Index = indexVec.size();

        CM_MATERIAL_TYPE meshMaterialType;
        if (mesh->materials.count == 0)
        {
            meshMaterialType = CM_MATERIAL_EMPTY;
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
            meshMaterialType = CM_MATERIAL_TEXTURE;
            surface.material.materialName = _strdup(mesh->materials.data[i]->name.data);
        }
        surface.material.materialType = meshMaterialType;

        size_t num_triangles = meshPart->num_triangles;
        customMapVertex* vertices = (customMapVertex*)calloc(num_triangles * 3, sizeof(customMapVertex));
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

                customMapVertex* vertex = &vertices[num_vertices++];

                ufbx_vec3 transformedPos = ufbx_transform_position(&meshMatrix, ufbx_get_vertex_vec3(&mesh->vertex_position, index));
                vertex->pos.x = static_cast<float>(transformedPos.x);
                vertex->pos.y = static_cast<float>(transformedPos.y);
                vertex->pos.z = static_cast<float>(transformedPos.z);

                
                switch (meshMaterialType)
                {
                case CM_MATERIAL_TEXTURE:
                case CM_MATERIAL_EMPTY:
                    vertex->color[0] = 1.0f;
                    vertex->color[1] = 1.0f;
                    vertex->color[2] = 1.0f;
                    vertex->color[3] = 1.0f;
                    break;
                case CM_MATERIAL_COLOUR:
                {
                    float factor = static_cast<float>(mesh->materials.data[i]->fbx.diffuse_factor.value_real);
                    vertex->color[0] = static_cast<float>(mesh->materials.data[i]->fbx.diffuse_color.value_vec3.x * factor);
                    vertex->color[1] = static_cast<float>(mesh->materials.data[i]->fbx.diffuse_color.value_vec3.y * factor);
                    vertex->color[2] = static_cast<float>(mesh->materials.data[i]->fbx.diffuse_color.value_vec3.z * factor);
                    vertex->color[3] = static_cast<float>(mesh->materials.data[i]->fbx.diffuse_color.value_vec4.w * factor);
                    break;
                }

                default:
                    _ASSERT(false);
                }
                


                // 1.0f - uv.v:
                // https://gamedev.stackexchange.com/questions/92886/fbx-uv-coordinates-is-strange
                vertex->texCoord[0] = (float)(ufbx_get_vertex_vec2(&mesh->vertex_uv, index).x);
                vertex->texCoord[1] = (float)(1.0f - ufbx_get_vertex_vec2(&mesh->vertex_uv, index).y);

                vertex->normal.x = static_cast<float>(ufbx_get_vertex_vec3(&mesh->vertex_normal, index).x);
                vertex->normal.y = static_cast<float>(ufbx_get_vertex_vec3(&mesh->vertex_normal, index).y);
                vertex->normal.z = static_cast<float>(ufbx_get_vertex_vec3(&mesh->vertex_normal, index).z);

                if (mesh->vertex_tangent.exists)
                {
                    vertex->tangent.x = static_cast<float>(ufbx_get_vertex_vec3(&mesh->vertex_tangent, index).x);
                    vertex->tangent.y = static_cast<float>(ufbx_get_vertex_vec3(&mesh->vertex_tangent, index).y);
                    vertex->tangent.z = static_cast<float>(ufbx_get_vertex_vec3(&mesh->vertex_tangent, index).z);
                }
                else
                {
                    vertex->tangent.x = 0.0f;
                    vertex->tangent.y = 0.0f;
                    vertex->tangent.z = 0.0f;
                }

                vertex->packedLmapCoord = 0;

                // possibly bitangent, unsure what the sign part means though
                vertex->binormalSign = 0.0f;
            }
        }

        _ASSERT(num_vertices == num_triangles * 3);

        // Generate the index buffer.
        ufbx_vertex_stream streams[1] = {
            {vertices, num_vertices, sizeof(customMapVertex)},
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

bool loadFBXModel(ufbx_node* node)
{
    customMapModel model;

    model.name = node->name.data;

    ufbx_transform origTransform = node->local_transform;
    origTransform.translation.x /= 100.0f;
    origTransform.translation.y /= 100.0f;
    origTransform.translation.z /= 100.0f;
    origTransform.scale.x /= 100.0f;
    origTransform.scale.y /= 100.0f;
    origTransform.scale.z /= 100.0f;
    ufbx_matrix meshMatrix = ufbx_transform_to_matrix(&origTransform);

    model.origin.x = static_cast<float>(node->local_transform.translation.x) / 100.0f;
    model.origin.y = static_cast<float>(node->local_transform.translation.y) / 100.0f;
    model.origin.z = static_cast<float>(node->local_transform.translation.z) / 100.0f;
    model.rotation.x = static_cast<float>(node->euler_rotation.x);
    model.rotation.y = static_cast<float>(node->euler_rotation.y);
    model.rotation.z = static_cast<float>(node->euler_rotation.z);
    model.scale = static_cast<float>(node->local_transform.scale.x) / 100.0f;

    if (model.scale == 0.0f)
    {
        printf("WARN: Ignoring model %s: has a scale of 0!\n", node->name.data);
        return false;
    }

    if (node->local_transform.scale.x != node->local_transform.scale.y || node->local_transform.scale.x != node->local_transform.scale.z)
        printf("WARNING: model %s uses non-uniform scaling! Only the X axis will be used for the scale value.\n", node->name.data);
    modelVec.push_back(model);

    return true;
}

void parseGFXData(ufbx_scene* scene, customMapInfo* projInfo)
{
    vertexVec.clear();
    indexVec.clear();
    surfaceVec.clear();
    modelVec.clear();
    hasTangentSpace = true;

    for (size_t i = 0; i < scene->nodes.count; i++)
    {
        ufbx_node* node = scene->nodes.data[i];

        switch (node->attrib_type)
        {
        case UFBX_ELEMENT_MESH:
            loadFBXMesh(node);
            break;
        case UFBX_ELEMENT_EMPTY:
            // loadFBXModel(node);
            break;
        default:
            //printf("ignoring node type %i: %s\n", node->attrib_type, node->name.data);
            break;
        }
    }
    projInfo->gfxInfo.surfaceCount = surfaceVec.size();
    projInfo->gfxInfo.vertexCount = vertexVec.size();
    projInfo->gfxInfo.indexCount = indexVec.size();
    projInfo->modelCount = modelVec.size();
    projInfo->gfxInfo.surfaces = new worldSurface[surfaceVec.size()];
    projInfo->gfxInfo.vertices = new customMapVertex[vertexVec.size()];
    projInfo->gfxInfo.indices = new uint16_t[indexVec.size()];
    projInfo->models = new customMapModel[modelVec.size()];
    memcpy(projInfo->gfxInfo.surfaces, &surfaceVec[0], surfaceVec.size() * sizeof(worldSurface));
    memcpy(projInfo->gfxInfo.vertices, &vertexVec[0], vertexVec.size() * sizeof(customMapVertex));
    memcpy(projInfo->gfxInfo.indices, &indexVec[0], indexVec.size() * sizeof(uint16_t));
    // memcpy(projInfo->models, &modelVec[0], modelVec.size() * sizeof(customMapModel));

    if (hasTangentSpace == false)
        printf("warning: one or more meshes have no tangent space. Be sure to select the tangent space box when exporting the FBX from blender.\n");
}

void parseCollisionData(ufbx_scene* scene, customMapInfo* projInfo)
{
    // hack: cbf changing the code for collision data, so just load collision dada as if it was gfx data

    vertexVec.clear();
    indexVec.clear();
    surfaceVec.clear();
    modelVec.clear();
    hasTangentSpace = true;

    for (size_t i = 0; i < scene->nodes.count; i++)
    {
        ufbx_node* node = scene->nodes.data[i];

        switch (node->attrib_type)
        {
        case UFBX_ELEMENT_MESH:
            loadFBXMesh(node);
            break;
        case UFBX_ELEMENT_EMPTY:
            // loadFBXModel(node);
            break;
        default:
            //printf("ignoring node type %i: %s\n", node->attrib_type, node->name.data);
            break;
        }
    }
    projInfo->colInfo.surfaceCount = surfaceVec.size();
    projInfo->colInfo.vertexCount = vertexVec.size();
    projInfo->colInfo.indexCount = indexVec.size();
    projInfo->modelCount = modelVec.size();
    projInfo->colInfo.surfaces = new worldSurface[surfaceVec.size()];
    projInfo->colInfo.vertices = new customMapVertex[vertexVec.size()];
    projInfo->colInfo.indices = new uint16_t[indexVec.size()];
    projInfo->models = new customMapModel[modelVec.size()];
    memcpy(projInfo->colInfo.surfaces, &surfaceVec[0], surfaceVec.size() * sizeof(worldSurface));
    memcpy(projInfo->colInfo.vertices, &vertexVec[0], vertexVec.size() * sizeof(customMapVertex));
    memcpy(projInfo->colInfo.indices, &indexVec[0], indexVec.size() * sizeof(uint16_t));
    // memcpy(projInfo->models, &modelVec[0], modelVec.size() * sizeof(customMapModel));

    if (hasTangentSpace == false)
        printf("warning: one or more meshes have no tangent space. Be sure to select the tangent space box when exporting the FBX from blender.\n");
}

customMapInfo* CustomMapInfo::createCustomMapInfo(std::string& projectName, ISearchPath& searchPath)
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
    ufbx_load_opts opts;
    opts.target_axes = ufbx_axes_right_handed_y_up;
    opts.generate_missing_normals = true;
    opts.allow_missing_vertex_position = false;
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

        ufbx_error error;
        ufbx_load_opts opts;
        opts.target_axes = ufbx_axes_right_handed_y_up;
        opts.generate_missing_normals = true;
        opts.allow_missing_vertex_position = false;
        colScene = ufbx_load_memory(colMapData, static_cast<size_t>(colFile.m_length), NULL, &error);
        if (!colScene)
        {
            fprintf(stderr, "Failed to load map collision fbx file: %s\n", error.description.data);
            return NULL;
        }
    }

    customMapInfo* projInfo = new customMapInfo;

    projInfo->name = projectName;
    projInfo->bspName = "maps/mp/" + projectName + ".d3dbsp";

    parseGFXData(gfxScene, projInfo);
    parseCollisionData(colScene, projInfo);

    ufbx_free_scene(gfxScene);
    if (gfxScene != colScene)
        ufbx_free_scene(colScene);

    return projInfo;
}
