#pragma once

#include <vector>
#include <ostream>

#include "Model/Obj/ObjCommon.h"
#include "Utils/DistinctMapper.h"

class ObjWriter
{
protected:
    struct ObjObjectData
    {
        DistinctMapper<ObjVertex> m_vertices;
        DistinctMapper<ObjNormal> m_normals;
        DistinctMapper<ObjUv> m_uvs;
        std::vector<ObjFace> m_faces;
    };

    struct ObjObjectDataOffsets
    {
        size_t vertexOffset;
        size_t normalOffset;
        size_t uvOffset;
    };

    std::string m_game_name;
    std::string m_zone_name;
    std::vector<ObjObject> m_objects;
    std::vector<ObjObjectData> m_object_data;
    std::vector<MtlMaterial> m_materials;

    void GetObjObjectDataOffsets(std::vector<ObjObjectDataOffsets>& inputOffsets, std::vector<ObjObjectDataOffsets>& distinctOffsets);

public:
    ObjWriter(std::string gameName, std::string zoneName);

    void AddObject(ObjObject object);
    void AddMaterial(MtlMaterial material);
    void AddVertex(int objectId, ObjVertex vertex);
    void AddNormal(int objectId, ObjNormal normal);
    void AddUv(int objectId, ObjUv uv);
    void AddFace(int objectId, ObjFace face);

    void WriteObj(std::ostream& stream);
    void WriteObj(std::ostream& stream, const std::string& mtlName);
    void WriteMtl(std::ostream& stream);
};
