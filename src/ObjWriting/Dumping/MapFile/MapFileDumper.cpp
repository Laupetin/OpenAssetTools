#include "MapFileDumper.h"

#include <cassert>
#include <cmath>
#include <iomanip>
#include <sstream>
#include <string_view>

namespace
{
    constexpr auto BRUSH_PLANE_POINT_DISTANCE = 64.0f;

    MapFileDumper::Vec3 CrossProduct(const MapFileDumper::Vec3& a, const MapFileDumper::Vec3& b)
    {
        return {
            a.m_y * b.m_z - a.m_z * b.m_y,
            a.m_z * b.m_x - a.m_x * b.m_z,
            a.m_x * b.m_y - a.m_y * b.m_x,
        };
    }

    MapFileDumper::Vec3 Normalize(const MapFileDumper::Vec3& value)
    {
        const auto length = std::sqrt(value.m_x * value.m_x + value.m_y * value.m_y + value.m_z * value.m_z);
        assert(length > 0.0f);

        if (length <= 0.0f)
            return {0.0f, 0.0f, 0.0f};

        return {value.m_x / length, value.m_y / length, value.m_z / length};
    }

    MapFileDumper::Vec3 Scale(const MapFileDumper::Vec3& value, const float factor)
    {
        return {value.m_x * factor, value.m_y * factor, value.m_z * factor};
    }

    MapFileDumper::Vec3 Add(const MapFileDumper::Vec3& a, const MapFileDumper::Vec3& b)
    {
        return {a.m_x + b.m_x, a.m_y + b.m_y, a.m_z + b.m_z};
    }

    float CleanZero(const float value)
    {
        return std::fabs(value) < 0.000001f ? 0.0f : value;
    }

    std::string FormatFloat(const float value)
    {
        std::ostringstream stream;
        stream << std::fixed << std::setprecision(6) << CleanZero(value);

        auto result = stream.str();
        constexpr std::string_view ZERO_FRACTION = ".000000";
        if (result.ends_with(ZERO_FRACTION))
            result.resize(result.size() - ZERO_FRACTION.size());

        return result;
    }
} // namespace

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

MapFileDumper::BrushPlane::BrushPlane(const Vec3 normal, const float distance)
    : m_normal(normal),
      m_distance(distance)
{
}

MapFileDumper::MapFileDumper(std::ostream& stream)
    : AbstractTextDumper(stream),
      m_flags{},
      m_entity_index(0u),
      m_brush_index(0u)
{
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

void MapFileDumper::WriteBrushPlane(const BrushPlane plane) const
{
    assert(m_flags.m_in_brush);

    const auto normalLength =
        std::sqrt(plane.m_normal.m_x * plane.m_normal.m_x + plane.m_normal.m_y * plane.m_normal.m_y + plane.m_normal.m_z * plane.m_normal.m_z);
    assert(normalLength > 0.0f);

    if (normalLength <= 0.0f)
        return;

    const auto normal = Normalize(plane.m_normal);
    const auto origin = Scale(normal, plane.m_distance / normalLength);
    const auto reference = std::fabs(normal.m_z) < 0.9f ? Vec3(0.0f, 0.0f, 1.0f) : Vec3(0.0f, 1.0f, 0.0f);
    const auto tangent = Normalize(CrossProduct(reference, normal));
    const auto bitangent = CrossProduct(normal, tangent);
    const auto point2 = Add(origin, Scale(tangent, BRUSH_PLANE_POINT_DISTANCE));
    const auto point3 = Add(origin, Scale(bitangent, BRUSH_PLANE_POINT_DISTANCE));

    Indent();
    m_stream << "( " << FormatFloat(origin.m_x) << " " << FormatFloat(origin.m_y) << " " << FormatFloat(origin.m_z) << " ) "
             << "( " << FormatFloat(point2.m_x) << " " << FormatFloat(point2.m_y) << " " << FormatFloat(point2.m_z) << " ) "
             << "( " << FormatFloat(point3.m_x) << " " << FormatFloat(point3.m_y) << " " << FormatFloat(point3.m_z) << " ) "
             << "clip_player 64 64 0 0 0 0 lightmap_gray 16384 16384 0 0 0 0\n";
}

void MapFileDumper::WritePhysicsBox(const PhysicsBox box)
{
    Indent();
    m_stream << "physics_box\n";
    Indent();
    m_stream << "{\n";
    IncIndent();

    Indent();
    m_stream << FormatFloat(box.m_orientation[0].m_x) << " " << FormatFloat(box.m_orientation[0].m_y) << " " << FormatFloat(box.m_orientation[0].m_z) << " "
             << FormatFloat(box.m_orientation[1].m_x) << " " << FormatFloat(box.m_orientation[1].m_y) << " " << FormatFloat(box.m_orientation[1].m_z) << " "
             << FormatFloat(box.m_orientation[2].m_x) << " " << FormatFloat(box.m_orientation[2].m_y) << " " << FormatFloat(box.m_orientation[2].m_z) << " "
             << FormatFloat(box.m_middle_point.m_x) << " " << FormatFloat(box.m_middle_point.m_y) << " " << FormatFloat(box.m_middle_point.m_z) << " "
             << FormatFloat(box.m_half_size.m_x) << " " << FormatFloat(box.m_half_size.m_y) << " " << FormatFloat(box.m_half_size.m_z) << "\n";

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
    m_stream << FormatFloat(cylinder.m_orientation.m_x) << " " << FormatFloat(cylinder.m_orientation.m_y) << " " << FormatFloat(cylinder.m_orientation.m_z)
             << " " << FormatFloat(cylinder.m_middle_point.m_x) << " " << FormatFloat(cylinder.m_middle_point.m_y) << " "
             << FormatFloat(cylinder.m_middle_point.m_z) << " " << FormatFloat(cylinder.m_height) << " " << FormatFloat(cylinder.m_radius) << "\n";

    DecIndent();
    Indent();
    m_stream << "}\n";
}
