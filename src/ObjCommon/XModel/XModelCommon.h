#pragma once

#include "Utils/DistinctMapper.h"

#include <optional>
#include <string>
#include <vector>

struct XModelQuaternion
{
    float x;
    float y;
    float z;
    float w;
};

struct XModelBone
{
    std::string name;
    std::optional<unsigned> parentIndex;
    float scale[3];
    float globalOffset[3];
    float localOffset[3];
    XModelQuaternion globalRotation;
    XModelQuaternion localRotation;
};

struct XModelBoneWeight
{
    unsigned boneIndex;
    float weight;
};

struct XModelVertexBoneWeightCollection
{
    std::vector<XModelBoneWeight> weights;
};

struct XModelVertexBoneWeights
{
    const XModelBoneWeight* weights;
    unsigned weightCount;
};

struct XModelVertex
{
    float coordinates[3];
    float normal[3];
    float color[4];
    float uv[2];
};

struct XModelFace
{
    unsigned vertexIndex[3];
};

struct XModelMaterial
{
    std::string name;
    std::string materialTypeName;
    float color[4];
    float transparency[4];
    float ambientColor[4];
    float incandescence[4];
    float coeffs[2];

    struct
    {
        float x;
        int y;
    } glow;

    struct
    {
        int x;
        float y;
    } refractive;

    float specularColor[4];
    float reflectiveColor[4];

    struct
    {
        int x;
        float y;
    } reflective;

    float blinn[2];
    float phong;
    std::string colorMapName;
    std::string normalMapName;
    std::string specularMapName;

    void ApplyDefaults();
};

struct XModelObject
{
    std::string name;
    unsigned materialIndex;
    std::vector<XModelFace> m_faces;
};

struct XModelCommon
{
    std::string m_name;
    std::vector<XModelObject> m_objects;
    std::vector<XModelBone> m_bones;
    std::vector<XModelMaterial> m_materials;
    std::vector<XModelVertex> m_vertices;
    std::vector<XModelVertexBoneWeights> m_vertex_bone_weights;
    XModelVertexBoneWeightCollection m_bone_weight_data;
};

struct VertexMergerPos
{
    float x;
    float y;
    float z;
    const XModelBoneWeight* weights;
    size_t weightCount;

    friend bool operator==(const VertexMergerPos& lhs, const VertexMergerPos& rhs);
    friend bool operator!=(const VertexMergerPos& lhs, const VertexMergerPos& rhs);
    friend bool operator<(const VertexMergerPos& lhs, const VertexMergerPos& rhs);
};

typedef DistinctMapper<VertexMergerPos> VertexMerger;
