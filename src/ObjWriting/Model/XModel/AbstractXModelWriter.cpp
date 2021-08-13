#include "AbstractXModelWriter.h"

AbstractXModelWriter::AbstractXModelWriter()
= default;

void AbstractXModelWriter::AddObject(XModelObject object)
{
    m_objects.emplace_back(std::move(object));
}

void AbstractXModelWriter::AddBone(XModelBone bone)
{
    m_bones.emplace_back(std::move(bone));
}

void AbstractXModelWriter::AddMaterial(XModelMaterial material)
{
    m_materials.emplace_back(std::move(material));
}

void AbstractXModelWriter::AddVertex(XModelVertex vertex)
{
    m_vertices.emplace_back(vertex);
}

void AbstractXModelWriter::AddFace(XModelFace face)
{
    m_faces.emplace_back(face);
}
