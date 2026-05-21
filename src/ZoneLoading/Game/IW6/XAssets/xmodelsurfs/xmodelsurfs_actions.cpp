#include "xmodelsurfs_actions.h"

#include <cassert>
#include <cstring>

using namespace IW6;

Actions_XModelSurfs::Actions_XModelSurfs(Zone& zone)
    : AssetLoadingActions(zone)
{
}

void Actions_XModelSurfs::ReallocGfxVertexUnion0(const GfxVertexUnion0* vertexUnion, XSurface* surface) const
{
    assert(vertexUnion);
    assert(surface);

    const unsigned int flags = surface->flags;

    const bool hasQuantized = (flags & 4) != 0;
    const bool hasMotion = ((flags >> 4) & 1) != 0;
    const bool useQuantizedVertexUnion = (flags & 3) == 0 && hasQuantized;

    if (useQuantizedVertexUnion)
    {
        auto* verts = m_zone.Memory().Alloc<GfxQuantizedVertex>(surface->vertCount);

        memcpy(verts, vertexUnion->quantizedVerts0, sizeof(GfxQuantizedVertex) * surface->vertCount);

        surface->verts0.quantizedVerts0 = verts;
    }
    else if (hasQuantized && hasMotion)
    {
        auto* verts = m_zone.Memory().Alloc<GfxQuantizedNoColorMotionVertex>(surface->vertCount);

        memcpy(verts, vertexUnion->quantizedNoColorMotionVerts0, sizeof(GfxQuantizedNoColorMotionVertex) * surface->vertCount);

        surface->verts0.quantizedNoColorMotionVerts0 = verts;
    }
    else if (hasQuantized)
    {
        auto* verts = m_zone.Memory().Alloc<GfxQuantizedNoColorVertex>(surface->vertCount);

        memcpy(verts, vertexUnion->quantizedNoColorVerts0, sizeof(GfxQuantizedNoColorVertex) * surface->vertCount);

        surface->verts0.quantizedNoColorVerts0 = verts;
    }
    else if (hasMotion)
    {
        auto* verts = m_zone.Memory().Alloc<GfxPackedMotionVertex>(surface->vertCount);

        memcpy(verts, vertexUnion->packedMotionVerts0, sizeof(GfxPackedMotionVertex) * surface->vertCount);

        surface->verts0.packedMotionVerts0 = verts;
    }
    else
    {
        auto* verts = m_zone.Memory().Alloc<GfxPackedVertex>(surface->vertCount);

        memcpy(verts, vertexUnion->packedVerts0, sizeof(GfxPackedVertex) * surface->vertCount);

        surface->verts0.packedVerts0 = verts;
    }
}

void Actions_XModelSurfs::ReallocXBlendInfo(const XBlendInfo* blendInfo, XSurface* surface) const
{
    assert(blendInfo);
    assert(surface);

    size_t blendVertsCount =
        2 * (7 * surface->blendVertCounts[3] + 5 * surface->blendVertCounts[2] + 3 * surface->blendVertCounts[1] + surface->blendVertCounts[0]);

    auto* blendVerts = m_zone.Memory().Alloc<XBlendInfo>(blendVertsCount);

    memcpy(blendVerts, blendInfo, sizeof(XBlendInfo) * blendVertsCount);

    surface->blendVerts = blendVerts;
}
