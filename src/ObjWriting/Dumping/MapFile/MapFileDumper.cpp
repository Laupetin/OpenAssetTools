#include "MapFileDumper.h"

#include <cassert>
#include <iomanip>

MapFileDumper::Vec3::Vec3(const float x, const float y, const float z)
    : v{}
{
    m_x = x;
    m_y = y;
    m_z = z;
}

MapFileDumper::Vec3::Vec3(float v[3])
    : v{v[0], v[1], v[2]}
{
}

MapFileDumper::PhysicsBox::PhysicsBox(const Vec3 middlePoint, const Vec3 halfSize, const Vec3 orientationX, const Vec3 orientationY, const Vec3 orientationZ)
    : m_middle_point(middlePoint),
      m_half_size(halfSize),
      m_orientation{orientationX, orientationY, orientationZ}
{
}

MapFileDumper::PhysicsCylinder::PhysicsCylinder(const Vec3 middlePoint, const float radius, const float height, const Vec3 orientation)
    : m_middle_point(middlePoint),
      m_radius(radius),
      m_height(height),
      m_orientation(orientation)
{
}

MapFileDumper::MapFileDumper(std::ostream& stream)
    : m_stream(stream),
      m_flags{},
      m_indent(0u),
      m_entity_index(0u),
      m_brush_index(0u)
{
}

void MapFileDumper::Indent() const
{
    for (auto i = 0u; i < m_indent; i++)
        m_stream << "  ";
}

void MapFileDumper::IncIndent()
{
    ++m_indent;
}

void MapFileDumper::DecIndent()
{
    assert(m_indent > 0);
    if (m_indent > 0)
        m_indent--;
}

void MapFileDumper::Init() const
{
    m_stream << "iwmap 4\n";
    m_stream << "\"000_Global\" flags active\n";
    m_stream << "\"The Map\" flags\n";
}

void MapFileDumper::WriteComment(const std::string& key) const
{
    Indent();
    m_stream << "// " << key << "\n";
}

void MapFileDumper::BeginEntity()
{
    assert(!m_flags.m_in_entity);
    if (m_flags.m_in_entity)
        return;

    Indent();
    m_stream << "// entity " << m_entity_index << "\n";

    Indent();
    m_stream << "{\n";

    IncIndent();
    m_entity_index++;
    m_brush_index = 0;
    m_flags.m_in_entity = true;
}

void MapFileDumper::EndEntity()
{
    assert(m_flags.m_in_entity);
    if (!m_flags.m_in_entity)
        return;

    DecIndent();
    Indent();
    m_stream << "}\n";
    m_flags.m_in_entity = false;
}

void MapFileDumper::BeginBrush()
{
    assert(m_flags.m_in_entity && !m_flags.m_in_brush);
    if (m_flags.m_in_brush)
        return;

    Indent();
    m_stream << "// brush " << m_brush_index << "\n";

    Indent();
    m_stream << "{\n";

    IncIndent();
    m_brush_index++;
    m_flags.m_in_brush = true;
}

void MapFileDumper::EndBrush()
{
    assert(m_flags.m_in_entity && m_flags.m_in_brush);
    if (!m_flags.m_in_brush)
        return;

    DecIndent();
    Indent();
    m_stream << "}\n";
    m_flags.m_in_brush = false;
}

void MapFileDumper::WriteKeyValue(const std::string& key, const std::string& value) const
{
    assert(m_flags.m_in_brush || m_flags.m_in_entity);

    Indent();
    m_stream << "\"" << key << "\" \"" << value << "\"\n";
}

void MapFileDumper::WritePhysicsBox(const PhysicsBox box)
{
    Indent();
    m_stream << "physics_box\n";
    Indent();
    m_stream << "{\n";
    IncIndent();

    Indent();
    m_stream << std::fixed << std::setprecision(6)
        << box.m_orientation[0].m_x << " " << box.m_orientation[0].m_y << " " << box.m_orientation[0].m_z
        << " " << box.m_orientation[1].m_x << " " << box.m_orientation[1].m_y << " " << box.m_orientation[1].m_z
        << " " << box.m_orientation[2].m_x << " " << box.m_orientation[2].m_y << " " << box.m_orientation[2].m_z
        << " " << box.m_middle_point.m_x << " " << box.m_middle_point.m_y << " " << box.m_middle_point.m_z
        << " " << box.m_half_size.m_x << " " << box.m_half_size.m_y << " " << box.m_half_size.m_z
        << "\n";

    DecIndent();
    Indent();
    m_stream << "}\n";
}

void MapFileDumper::WritePhysicsCylinder(PhysicsCylinder cylinder)
{
    Indent();
    m_stream << "physics_cylinder\n";
    Indent();
    m_stream << "{\n";
    IncIndent();

    Indent();
    m_stream << std::fixed << std::setprecision(6)
        << cylinder.m_orientation.m_x << " " << cylinder.m_orientation.m_y << " " << cylinder.m_orientation.m_z
        << " " << cylinder.m_middle_point.m_x << " " << cylinder.m_middle_point.m_y << " " << cylinder.m_middle_point.m_z
        << " " << cylinder.m_height << " " << cylinder.m_radius
        << "\n";

    DecIndent();
    Indent();
    m_stream << "}\n";
}
