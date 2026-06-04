#include "XAnimCommon.h"

#include <algorithm>
#include <format>
#include <numeric>
#include <utility>
#include <vector>

namespace xanim
{
    CommonXQuat::CommonXQuat()
        : value{}
    {
    }

    CommonXQuat::CommonXQuat(const int16_t v0, const int16_t v1, const int16_t v2, const int16_t v3)
        : value{v0, v1, v2, v3}
    {
    }

    CommonXQuat2::CommonXQuat2()
        : value{}
    {
    }

    CommonXQuat2::CommonXQuat2(const int16_t v0, const int16_t v1)
        : value{v0, v1}
    {
    }

    CommonVec3U8::CommonVec3U8()
        : value{}
    {
    }

    CommonVec3U8::CommonVec3U8(const uint8_t x, const uint8_t y, const uint8_t z)
        : value{x, y, z}
    {
    }

    CommonVec3U16::CommonVec3U16()
        : value{}
    {
    }

    CommonVec3U16::CommonVec3U16(const uint16_t x, const uint16_t y, const uint16_t z)
        : value{x, y, z}
    {
    }

    QuatTrack::QuatTrack()
        : m_type(QuatType::NO_QUAT)
    {
    }

    TransTrack::TransTrack()
        : m_type(TransType::NO_TRANS),
          m_mins({}),
          m_size({}),
          m_constant({})
    {
    }

    CommonXAnimNotifyInfo::CommonXAnimNotifyInfo()
        : m_time(0)
    {
    }

    CommonXAnimNotifyInfo::CommonXAnimNotifyInfo(std::string name, const float time)
        : m_name(std::move(name)),
          m_time(time)
    {
    }

    CommonDeltaTransTrack::CommonDeltaTransTrack()
        : m_constant(std::nullopt),
          m_small_trans(false),
          m_mins({}),
          m_size({})
    {
    }

    CommonXAnimParts::CommonXAnimParts()
        : m_num_frames(0),
          m_looped(false),
          m_frame_rate(0),
          m_asset_type(0)
    {
    }

    void CommonXAnimParts::SortBoneTracksForQuats()
    {
        std::vector<size_t> boneOrder(m_bone_tracks.size());
        std::ranges::iota(boneOrder, 0);

        std::ranges::sort(boneOrder,
                          [this](const size_t i0, const size_t i1)
                          {
                              const auto type0 = std::to_underlying(m_bone_tracks[i0].m_quat.m_type);
                              const auto type1 = std::to_underlying(m_bone_tracks[i1].m_quat.m_type);
                              if (type0 != type1)
                                  return type0 < type1;

                              return i0 < i1;
                          });

        std::vector<BoneTrack> boneTrackCopies(m_bone_tracks.size());
        for (size_t i = 0u; i < boneOrder.size(); ++i)
        {
            boneTrackCopies[i] = std::move(m_bone_tracks[boneOrder[i]]);
        }

        m_bone_tracks = std::move(boneTrackCopies);
    }

    std::vector<size_t> CommonXAnimParts::GetBoneTrackOrderForTrans() const
    {
        // This assumes the bone tracks were already sorted for quats
        std::vector<size_t> boneOrder(m_bone_tracks.size());
        std::ranges::iota(boneOrder, 0);

        std::ranges::sort(boneOrder,
                          [this](const size_t i0, const size_t i1)
                          {
                              const auto type0 = std::to_underlying(m_bone_tracks[i0].m_trans.m_type);
                              const auto type1 = std::to_underlying(m_bone_tracks[i1].m_trans.m_type);
                              if (type0 != type1)
                                  return type0 < type1;

                              return i0 < i1;
                          });

        return boneOrder;
    }

    std::string GetCompiledFileNameForAssetName(const std::string& assetName)
    {
        return std::format("xanim/{}", assetName);
    }
} // namespace xanim
