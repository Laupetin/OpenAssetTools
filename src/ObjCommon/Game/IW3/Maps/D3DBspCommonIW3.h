#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

namespace IW3::d3dbsp
{
    inline constexpr std::array<char, 4> BSP_MAGIC{'I', 'B', 'S', 'P'};
    inline constexpr uint32_t BSP_VERSION = 22u;
    inline constexpr uint32_t RUNTIME_MATERIAL_CONTENT_MASK = 0xdffffffbu;
    inline constexpr uint32_t RAW_BSP_BRUSH_CONTENT_MARKER = 0x20000000u;

    // IW3 v22 stores primary lights as DiskPrimaryLight records. The linker
    // converts these to runtime ComPrimaryLight records and derives
    // cosHalfFovExpanded while loading.
    inline constexpr size_t RAW_PRIMARY_LIGHT_SIZE = 128uz;

    // Synthesized from cod4map/linker_pc/Radiant loader usage. IW3 v22 stores
    // some render geometry twice: layered data in the low lump range and simple
    // data in the later lump range.
    enum class LumpType : uint32_t
    {
        LUMP_MATERIALS = 0,
        LUMP_LIGHTMAPS = 1,
        LUMP_LIGHTGRID_ENTRIES = 2,
        LUMP_LIGHTGRID_COLORS = 3,
        LUMP_PLANES = 4,
        LUMP_BRUSHSIDES = 5,
        LUMP_BRUSHSIDE_EDGE_COUNTS = 6,
        LUMP_BRUSHEDGES = 7,
        LUMP_BRUSHES = 8,

        // Layered world geometry. LUMP_VERTEX_LAYER_DATA is the extra payload
        // paired with LUMP_LAYERED_VERTS.
        LUMP_LAYERED_TRI_SOUPS = 9,
        LUMP_LAYERED_VERTS = 10,
        LUMP_LAYERED_INDICES = 11,
        LUMP_CULLGROUPS = 12,
        LUMP_CULLGROUP_INDICES = 13,

        LUMP_OBSOLETE_1 = 14,
        LUMP_OBSOLETE_2 = 15,
        LUMP_OBSOLETE_3 = 16,
        LUMP_OBSOLETE_4 = 17,
        LUMP_OBSOLETE_5 = 18,
        LUMP_PORTALVERTS = 19,
        LUMP_OBSOLETE_6 = 20,
        LUMP_UINDS = 21,
        LUMP_BRUSHVERTSCOUNTS = 22,
        LUMP_BRUSHVERTS = 23,
        LUMP_LAYERED_AABBTREES = 24,
        LUMP_CELLS = 25,
        LUMP_PORTALS = 26,
        LUMP_NODES = 27,
        LUMP_LEAFS = 28,
        LUMP_LEAFBRUSHES = 29,
        LUMP_LEAFSURFACES = 30,
        LUMP_COLLISIONVERTS = 31,
        LUMP_COLLISIONTRIS = 32,
        LUMP_COLLISION_EDGE_WALKABLE = 33,
        LUMP_COLLISIONBORDERS = 34,
        LUMP_COLLISIONPARTITIONS = 35,
        LUMP_COLLISIONAABBS = 36,
        LUMP_MODELS = 37,
        LUMP_VISIBILITY = 38,      // Optional PVS data; loaders can fall back when it is absent.
        LUMP_ENTITIES = 39,        // Raw entity text consumed by linker and MapEnts.
        LUMP_PATHCONNECTIONS = 40, // SP path data; absent for MP maps.
        LUMP_REFLECTION_PROBES = 41,
        LUMP_VERTEX_LAYER_DATA = 42,
        LUMP_PRIMARY_LIGHTS = 43,
        LUMP_LIGHTGRID_HEADER = 44,
        LUMP_LIGHTGRID_ROWS = 45,
        LUMP_OBSOLETE_10 = 46,

        // Simple/non-layered world geometry. IW3 v22 keeps this alongside the
        // layered set so the linker/Radiant can choose the appropriate path.
        LUMP_SIMPLE_TRI_SOUPS = 47,
        LUMP_SIMPLE_VERTS = 48,
        LUMP_SIMPLE_INDICES = 49,
        LUMP_SIMPLE_CULLGROUPS = 50,
        LUMP_SIMPLE_AABBTREES = 51,
        LUMP_LIGHT_REGION_COUNTS = 52,
        LUMP_LIGHT_REGION_HULLS = 53,
        LUMP_LIGHT_REGION_AXES = 54,
    };

    // Matches the stock raw BSP loader's TrisType enum. IW3 v22 can contain
    // both layered and unlayered render geometry; linker_pc/Radiant select one
    // path before loading surfaces, verts, indices, cull groups, and AABB trees.
    enum class TrisType : uint32_t
    {
        TRIS_TYPE_LAYERED = 0,
        TRIS_TYPE_SIMPLE = 1,
        TRIS_TYPE_COUNT = 2,
    };

    // IW3 v22 d3dbsp files use this order in the stock tools output.
    inline constexpr std::array LUMP_WRITE_ORDER{
        LumpType::LUMP_MATERIALS,
        LumpType::LUMP_LIGHTMAPS,
        LumpType::LUMP_LIGHTGRID_HEADER,
        LumpType::LUMP_LIGHTGRID_ROWS,
        LumpType::LUMP_LIGHTGRID_ENTRIES,
        LumpType::LUMP_LIGHTGRID_COLORS,
        LumpType::LUMP_PLANES,
        LumpType::LUMP_BRUSHSIDES,
        LumpType::LUMP_BRUSHSIDE_EDGE_COUNTS,
        LumpType::LUMP_BRUSHEDGES,
        LumpType::LUMP_BRUSHES,
        LumpType::LUMP_LAYERED_TRI_SOUPS,
        LumpType::LUMP_LAYERED_VERTS,
        LumpType::LUMP_VERTEX_LAYER_DATA,
        LumpType::LUMP_LAYERED_INDICES,
        LumpType::LUMP_CULLGROUPS,
        LumpType::LUMP_CULLGROUP_INDICES,
        LumpType::LUMP_PORTALVERTS,
        LumpType::LUMP_LAYERED_AABBTREES,
        LumpType::LUMP_CELLS,
        LumpType::LUMP_PORTALS,
        LumpType::LUMP_NODES,
        LumpType::LUMP_LEAFS,
        LumpType::LUMP_LEAFBRUSHES,
        LumpType::LUMP_LEAFSURFACES,
        LumpType::LUMP_COLLISIONVERTS,
        LumpType::LUMP_COLLISIONTRIS,
        LumpType::LUMP_COLLISION_EDGE_WALKABLE,
        LumpType::LUMP_COLLISIONBORDERS,
        LumpType::LUMP_COLLISIONPARTITIONS,
        LumpType::LUMP_COLLISIONAABBS,
        LumpType::LUMP_MODELS,
        LumpType::LUMP_VISIBILITY,
        LumpType::LUMP_ENTITIES,
        LumpType::LUMP_PRIMARY_LIGHTS,
        LumpType::LUMP_LIGHT_REGION_COUNTS,
        LumpType::LUMP_LIGHT_REGION_HULLS,
        LumpType::LUMP_LIGHT_REGION_AXES,
        LumpType::LUMP_SIMPLE_TRI_SOUPS,
        LumpType::LUMP_SIMPLE_VERTS,
        LumpType::LUMP_SIMPLE_INDICES,
        LumpType::LUMP_SIMPLE_CULLGROUPS,
        LumpType::LUMP_SIMPLE_AABBTREES,
        LumpType::LUMP_PATHCONNECTIONS,
        LumpType::LUMP_REFLECTION_PROBES,
    };
} // namespace IW3::d3dbsp
