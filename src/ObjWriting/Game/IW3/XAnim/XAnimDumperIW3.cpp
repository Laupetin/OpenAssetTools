#include "XAnimDumperIW3.h"

#include "XAnim/CompiledXAnimWriter.h"
#include "XAnim/FlatXAnimReader.h"
#include "XAnim/XAnimCommon.h"

#include <array>
#include <cassert>
#include <cstdint>
#include <expected>
#include <format>
#include <limits>
#include <optional>
#include <string>
#include <vector>

using namespace xanim;
using namespace IW3;

namespace
{
    [[nodiscard]] const std::string& ResolveScriptString(const ScriptString value, const XAssetInfo<XAnimParts>& asset)
    {
        assert(asset.m_zone != nullptr && value < asset.m_zone->m_script_strings.Count());
        return asset.m_zone->m_script_strings[value];
    }

    [[nodiscard]] uint16_t GetNumLoopFrames(const XAnimParts& parts)
    {
        assert(parts.numframes < std::numeric_limits<uint16_t>::max());
        // Raw non-looped xanims store numframes + 1 in keyed track counts/header fields.
        return static_cast<uint16_t>(parts.numframes + 1u);
    }

    [[nodiscard]] bool UseByteIndices(const XAnimParts& parts)
    {
        return parts.numframes < 256;
    }

    std::vector<CommonXAnimNotifyInfo> ConvertNotifies(const XAnimParts& parts, const XAssetInfo<AssetXAnim::Type>& assetInfo)
    {
        std::vector<CommonXAnimNotifyInfo> result;
        if (!parts.notify || parts.notifyCount == 0)
            return result;

        for (auto i = 0u; i < parts.notifyCount; i++)
        {
            const auto& notify = parts.notify[i];
            CommonXAnimNotifyInfo commonNotify;

            commonNotify.m_name = ResolveScriptString(notify.name, assetInfo);
            commonNotify.m_time = notify.time;

            result.emplace_back(std::move(commonNotify));
        }

        return result;
    }

    CommonDeltaQuatTrack ConvertDeltaQuatTrack(const XAnimDeltaPartQuat& deltaQuat, const bool useByteIndices, const uint16_t numLoopFrames)
    {
        CommonDeltaQuatTrack result;
        if (deltaQuat.size > 0)
        {
            const auto frameCount = static_cast<size_t>(deltaQuat.size) + 1uz;
            result.m_frames2.reserve(frameCount);
            result.m_indices.reserve(frameCount);

            for (auto i = 0uz; i < frameCount; i++)
                result.m_frames2.emplace_back(deltaQuat.u.frames.frames[i].value[0], deltaQuat.u.frames.frames[i].value[1]);

            if (useByteIndices)
            {
                for (auto i = 0uz; i < frameCount; i++)
                    result.m_indices.emplace_back(deltaQuat.u.frames.indices._1[i]);
            }
            else
            {
                for (auto i = 0uz; i < frameCount; i++)
                    result.m_indices.emplace_back(deltaQuat.u.frames.indices._2[i]);
            }

            assert(result.m_indices.size() <= numLoopFrames);
        }
        else
        {
            result.m_frames2.emplace_back(deltaQuat.u.frame0.value[0], deltaQuat.u.frame0.value[1]);
        }

        return result;
    }

    CommonDeltaTransTrack ConvertDeltaTransTrack(const XAnimPartTrans& deltaTrans, const bool useByteIndices)
    {
        CommonDeltaTransTrack result;
        if (deltaTrans.size > 0)
        {
            result.m_small_trans = deltaTrans.smallTrans;
            result.m_mins = {
                deltaTrans.u.frames.mins.x,
                deltaTrans.u.frames.mins.y,
                deltaTrans.u.frames.mins.z,
            };
            result.m_size = {
                deltaTrans.u.frames.size.x,
                deltaTrans.u.frames.size.y,
                deltaTrans.u.frames.size.z,
            };

            const auto frameCount = static_cast<size_t>(deltaTrans.size) + 1uz;
            result.m_indices.reserve(frameCount);
            if (useByteIndices)
            {
                for (auto i = 0uz; i < frameCount; i++)
                    result.m_indices.emplace_back(static_cast<uint8_t>(deltaTrans.u.frames.indices._1[i]));
            }
            else
            {
                for (auto i = 0uz; i < frameCount; i++)
                    result.m_indices.emplace_back(deltaTrans.u.frames.indices._2[i]);
            }

            if (deltaTrans.smallTrans)
            {
                result.m_frames_u8.reserve(frameCount);
                for (auto i = 0uz; i < frameCount; i++)
                {
                    result.m_frames_u8.emplace_back(
                        deltaTrans.u.frames.frames._1[i][0], deltaTrans.u.frames.frames._1[i][1], deltaTrans.u.frames.frames._1[i][2]);
                }
            }
            else
            {
                result.m_frames_u16.reserve(frameCount);
                for (auto i = 0uz; i < frameCount; i++)
                {
                    result.m_frames_u16.emplace_back(
                        deltaTrans.u.frames.frames._2[i][0], deltaTrans.u.frames.frames._2[i][1], deltaTrans.u.frames.frames._2[i][2]);
                }
            }
        }
        else
        {
            result.m_constant = std::array<float, 3>({
                deltaTrans.u.frame0.v[0],
                deltaTrans.u.frame0.v[1],
                deltaTrans.u.frame0.v[2],
            });
        }

        return result;
    }

    std::unique_ptr<CommonXAnimDeltaTrack> ConvertDeltaTrack(const XAnimParts& parts, const bool useByteIndices, const uint16_t numLoopFrames)
    {
        if (!parts.deltaPart)
            return nullptr;

        auto result = std::make_unique<CommonXAnimDeltaTrack>();

        assert(static_cast<bool>(parts.deltaPart) == static_cast<bool>(parts.bDelta));
        if (!parts.deltaPart)
            return result;

        if (parts.deltaPart->quat)
            result->m_quat = ConvertDeltaQuatTrack(*parts.deltaPart->quat, useByteIndices, numLoopFrames);

        if (parts.deltaPart->trans)
            result->m_trans = ConvertDeltaTransTrack(*parts.deltaPart->trans, useByteIndices);

        return result;
    }
} // namespace

namespace xanim
{
    void DumperIW3::DumpAsset(AssetDumpingContext& context, const XAssetInfo<AssetXAnim::Type>& asset)
    {
        const auto* parts = asset.Asset();

        const auto useByteIndices = UseByteIndices(*parts);
        const auto numLoopFrames = GetNumLoopFrames(*parts);

        std::vector<std::string> boneNames;
        if (parts->names)
        {
            boneNames.reserve(parts->boneCount[PART_TYPE_ALL]);
            for (auto i = 0u; i < parts->boneCount[PART_TYPE_ALL]; i++)
                boneNames.emplace_back(asset.m_zone->m_script_strings.Value(parts->names[i]));
        }

        const XAnimBoneCounts boneCounts(parts->boneCount[PART_TYPE_NO_QUAT],
                                         parts->boneCount[PART_TYPE_HALF_QUAT],
                                         parts->boneCount[PART_TYPE_FULL_QUAT],
                                         parts->boneCount[PART_TYPE_HALF_QUAT_NO_SIZE],
                                         parts->boneCount[PART_TYPE_FULL_QUAT_NO_SIZE],
                                         parts->boneCount[PART_TYPE_SMALL_TRANS],
                                         parts->boneCount[PART_TYPE_TRANS],
                                         parts->boneCount[PART_TYPE_TRANS_NO_SIZE],
                                         parts->boneCount[PART_TYPE_NO_TRANS]);

        // Root indices should only ever be used when it is !useByteIndices, therefore we should be safe to always use the short version
        assert(!useByteIndices || parts->indices._1 == nullptr);
        FlatXAnimReadCursor flatData(parts->dataByte,
                                     parts->dataByteCount,
                                     parts->dataShort,
                                     parts->dataShortCount,
                                     parts->dataInt,
                                     parts->dataIntCount,
                                     parts->randomDataByte,
                                     parts->randomDataByteCount,
                                     parts->randomDataShort,
                                     parts->randomDataShortCount,
                                     parts->indices._2,
                                     parts->indexCount);

        auto maybeBoneTracks = CreateBoneTracksFromFlatData(std::move(boneNames), boneCounts, flatData, useByteIndices);
        if (!maybeBoneTracks.has_value())
        {
            con::error("Failed to reconstruct bone tracks from XAnim {}: {}", parts->name, maybeBoneTracks.error());
            return;
        }

        const auto assetFile = context.OpenAssetFile(GetCompiledFileNameForAssetName(asset.m_name));
        if (!assetFile)
            return;

        CommonXAnimParts commonParts;
        commonParts.m_num_frames = parts->numframes;
        commonParts.m_looped = parts->bLoop;
        commonParts.m_frame_rate = parts->framerate;
        commonParts.m_asset_type = parts->assetType;
        commonParts.m_bone_tracks = std::move(maybeBoneTracks).value();
        commonParts.m_notifies = ConvertNotifies(*parts, asset);
        commonParts.m_delta_track = ConvertDeltaTrack(*parts, useByteIndices, numLoopFrames);

        WriteCompiledXAnim(*assetFile, commonParts);
    }
} // namespace xanim
