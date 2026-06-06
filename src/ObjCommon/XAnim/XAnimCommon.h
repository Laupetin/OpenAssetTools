#pragma once

#include <array>
#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace xanim
{
    enum class QuatType : uint8_t
    {
        NO_QUAT = 0,
        HALF_QUAT = 1,
        FULL_QUAT = 2,
        HALF_QUAT_NO_SIZE = 3,
        FULL_QUAT_NO_SIZE = 4,
    };

    enum class TransType : uint8_t
    {
        SMALL_TRANS = 5,
        FULL_TRANS = 6,
        TRANS_NO_SIZE = 7,
        NO_TRANS = 8,
    };

    struct CommonXQuat
    {
        CommonXQuat();
        CommonXQuat(int16_t v0, int16_t v1, int16_t v2, int16_t v3);

        int16_t value[4];
    };

    struct CommonXQuat2
    {
        CommonXQuat2();
        CommonXQuat2(int16_t v0, int16_t v1);

        int16_t value[2];
    };

    struct CommonVec3U8
    {
        CommonVec3U8();
        CommonVec3U8(uint8_t x, uint8_t y, uint8_t z);

        uint8_t value[3];
    };

    struct CommonVec3U16
    {
        CommonVec3U16();
        CommonVec3U16(uint16_t x, uint16_t y, uint16_t z);

        uint16_t value[3];
    };

    class QuatTrack
    {
    public:
        QuatTrack();

        QuatType m_type;
        std::vector<uint16_t> m_indices;
        std::vector<CommonXQuat> m_frames;
        std::vector<CommonXQuat2> m_frames2;
    };

    class TransTrack
    {
    public:
        TransTrack();

        TransType m_type;
        std::vector<uint16_t> m_indices;
        std::array<float, 3> m_mins;
        std::array<float, 3> m_size;
        std::vector<CommonVec3U8> m_frames_u8;
        std::vector<CommonVec3U16> m_frames_u16;
        std::array<float, 3> m_constant;
    };

    class BoneTrack
    {
    public:
        BoneTrack() = default;

        std::string m_name;
        QuatTrack m_quat;
        TransTrack m_trans;
    };

    class CommonXAnimNotifyInfo
    {
    public:
        CommonXAnimNotifyInfo();
        CommonXAnimNotifyInfo(std::string name, float time);

        std::string m_name;
        float m_time;
    };

    class CommonDeltaQuatTrack
    {
    public:
        CommonDeltaQuatTrack() = default;

        [[nodiscard]] bool Is3DTrack() const;

        std::vector<uint16_t> m_indices;
        std::vector<CommonXQuat> m_frames;
        std::vector<CommonXQuat2> m_frames2;
    };

    class CommonDeltaTransTrack
    {
    public:
        CommonDeltaTransTrack();

        std::optional<std::array<float, 3>> m_constant;

        bool m_small_trans;
        std::vector<uint16_t> m_indices;
        std::array<float, 3> m_mins;
        std::array<float, 3> m_size;
        std::vector<CommonVec3U8> m_frames_u8;
        std::vector<CommonVec3U16> m_frames_u16;
    };

    class CommonXAnimDeltaTrack
    {
    public:
        CommonXAnimDeltaTrack() = default;

        std::optional<CommonDeltaQuatTrack> m_quat;
        std::optional<CommonDeltaTransTrack> m_trans;
    };

    class CommonXAnimParts
    {
    public:
        CommonXAnimParts();

        void SortBoneTracksForQuats();
        [[nodiscard]] std::vector<size_t> GetBoneTrackOrderForTrans() const;

        size_t m_num_frames;
        bool m_looped;
        bool m_left_hand_grip_ik;
        bool m_streamable;
        float m_frame_rate;
        float m_primed_length;
        uint8_t m_asset_type;
        std::vector<BoneTrack> m_bone_tracks;
        std::vector<CommonXAnimNotifyInfo> m_notifies;
        std::unique_ptr<CommonXAnimDeltaTrack> m_delta_track;
    };

    [[nodiscard]] std::string GetCompiledFileNameForAssetName(const std::string& assetName);
} // namespace xanim
