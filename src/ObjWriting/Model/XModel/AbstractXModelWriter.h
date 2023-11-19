#pragma once

#include "Model/XModel/XModelCommon.h"

#include <vector>

class AbstractXModelWriter
{
protected:
    std::vector<XModelObject> m_objects;
    std::vector<XModelBone> m_bones;
    std::vector<XModelMaterial> m_materials;
    std::vector<XModelVertex> m_vertices;
    std::vector<XModelVertexBoneWeights> m_vertex_bone_weights;
    std::vector<XModelFace> m_faces;

public:
    AbstractXModelWriter();

    void AddObject(XModelObject object);
    void AddBone(XModelBone bone);
    void AddMaterial(XModelMaterial material);
    void AddVertex(XModelVertex vertex);
    void AddVertexBoneWeights(XModelVertexBoneWeights vertexBoneWeights);
    void AddFace(XModelFace face);
};
