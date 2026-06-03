#include "XAnimLoaderIW3.h"

#include "Utils/Logging/Log.h"
#include "XAnim/CompiledXAnimLoader.h"
#include "XAnim/FlatXAnimDataWriter.h"
#include "XAnim/XAnimCommon.h"

#include <algorithm>
#include <array>
#include <cassert>
#include <cstdint>
#include <cstring>
#include <format>
#include <limits>
#include <string>
#include <utility>
#include <vector>

using namespace IW3;

namespace
{
    void ConvertNoteTracks(XAnimParts& parts,
                           const xanim::CommonXAnimParts& commonParts,
                           AssetRegistration<AssetXAnim>& registration,
                           MemoryManager& memory,
                           ZoneScriptStrings& scriptStrings)
    {
        if (commonParts.m_notifies.empty())
            return;

        const auto numCommonNoteTracks = commonParts.m_notifies.size();
        const auto numNoteTracks = static_cast<uint8_t>(std::min<size_t>(numCommonNoteTracks, std::numeric_limits<uint8_t>::max()));

        if (numNoteTracks < numCommonNoteTracks)
            con::error("XAnim {}: Could only fit {} of {} notetracks", parts.name, numNoteTracks, numCommonNoteTracks);

        parts.notifyCount = numNoteTracks;
        parts.notify = memory.Alloc<XAnimNotifyInfo>(numNoteTracks);

        for (auto notifyIndex = 0u; notifyIndex < numCommonNoteTracks; notifyIndex++)
        {
            const auto& commonNotify = commonParts.m_notifies[notifyIndex];
            auto& notify = parts.notify[notifyIndex];

            notify.name = scriptStrings.AddOrGetScriptString(commonNotify.m_name);
            registration.AddScriptString(notify.name);

            notify.time = commonNotify.m_time;
        }
    }

    template<typename T> void ConvertIndices(T& indices, const std::vector<uint16_t>& commonIndices, const bool useByteIndices)
    {
        if (useByteIndices)
        {
            const auto numIndices = commonIndices.size();
            for (size_t i = 0u; i < numIndices; i++)
            {
                assert(commonIndices[i] <= std::numeric_limits<uint8_t>::max());
                indices._1[i] = static_cast<uint8_t>(commonIndices[i]);
            }
        }
        else
        {
            std::memcpy(indices._2, commonIndices.data(), commonIndices.size() * sizeof(uint16_t));
        }
    }

    void CountBoneTrackTypes(XAnimParts& parts, const xanim::BoneTrack& boneTrack)
    {
        switch (boneTrack.m_quat.m_type)
        {
        case xanim::QuatType::NO_QUAT:
            parts.boneCount[PART_TYPE_NO_QUAT]++;
            break;
        case xanim::QuatType::HALF_QUAT:
            parts.boneCount[PART_TYPE_HALF_QUAT]++;
            break;
        case xanim::QuatType::FULL_QUAT:
            parts.boneCount[PART_TYPE_FULL_QUAT]++;
            break;
        case xanim::QuatType::HALF_QUAT_NO_SIZE:
            parts.boneCount[PART_TYPE_HALF_QUAT_NO_SIZE]++;
            break;
        case xanim::QuatType::FULL_QUAT_NO_SIZE:
            parts.boneCount[PART_TYPE_FULL_QUAT_NO_SIZE]++;
            break;
        }

        switch (boneTrack.m_trans.m_type)
        {
        case xanim::TransType::SMALL_TRANS:
            parts.boneCount[PART_TYPE_SMALL_TRANS]++;
            break;
        case xanim::TransType::FULL_TRANS:
            parts.boneCount[PART_TYPE_TRANS]++;
            break;
        case xanim::TransType::TRANS_NO_SIZE:
            parts.boneCount[PART_TYPE_TRANS_NO_SIZE]++;
            break;
        case xanim::TransType::NO_TRANS:
            parts.boneCount[PART_TYPE_NO_TRANS]++;
            break;
        }
    }

    void ConvertFlatData(XAnimParts& parts, const xanim::FlatData& flatData, MemoryManager& memory)
    {
        if (!flatData.m_data_byte.empty())
        {
            parts.dataByteCount = static_cast<uint16_t>(flatData.m_data_byte.size());
            parts.dataByte = memory.Alloc<uint8_t>(parts.dataByteCount);
            std::memcpy(parts.dataByte, flatData.m_data_byte.data(), parts.dataByteCount * sizeof(uint8_t));
        }

        if (!flatData.m_data_short.empty())
        {
            parts.dataShortCount = static_cast<uint16_t>(flatData.m_data_short.size());
            parts.dataShort = memory.Alloc<int16_t>(parts.dataShortCount);
            std::memcpy(parts.dataShort, flatData.m_data_short.data(), parts.dataShortCount * sizeof(int16_t));
        }

        if (!flatData.m_data_int.empty())
        {
            parts.dataIntCount = static_cast<uint16_t>(flatData.m_data_int.size());
            parts.dataInt = memory.Alloc<int32_t>(parts.dataIntCount);
            std::memcpy(parts.dataInt, flatData.m_data_int.data(), parts.dataIntCount * sizeof(int32_t));
        }

        if (!flatData.m_random_data_byte.empty())
        {
            parts.randomDataByteCount = static_cast<uint16_t>(flatData.m_random_data_byte.size());
            parts.randomDataByte = memory.Alloc<uint8_t>(parts.randomDataByteCount);
            std::memcpy(parts.randomDataByte, flatData.m_random_data_byte.data(), parts.randomDataByteCount * sizeof(uint8_t));
        }

        if (!flatData.m_random_data_short.empty())
        {
            parts.randomDataShortCount = static_cast<unsigned int>(flatData.m_random_data_short.size());
            parts.randomDataShort = memory.Alloc<int16_t>(parts.randomDataShortCount);
            std::memcpy(parts.randomDataShort, flatData.m_random_data_short.data(), parts.randomDataShortCount * sizeof(int16_t));
        }

        if (!flatData.m_indices.empty())
        {
            parts.indexCount = static_cast<unsigned int>(flatData.m_indices.size());
            parts.indices._2 = memory.Alloc<uint16_t>(parts.indexCount);
            std::memcpy(parts.indices._2, flatData.m_indices.data(), parts.indexCount * sizeof(uint16_t));
        }
    }

    void ConvertCommonDeltaQuatPart(XAnimDeltaPart& deltaPart,
                                    const xanim::CommonDeltaQuatTrack& commonDeltaQuatTrack,
                                    MemoryManager& memory,
                                    const bool useByteIndices)
    {
        if (commonDeltaQuatTrack.m_frames2.size() == 1)
        {
            deltaPart.quat = static_cast<XAnimDeltaPartQuat*>(memory.AllocRaw(offsetof(XAnimDeltaPartQuat, u) + sizeof(XAnimDeltaPartQuatData::frame0)));
            deltaPart.quat->size = 0;
            deltaPart.quat->u.frame0.value[0] = commonDeltaQuatTrack.m_frames2[0].value[0];
            deltaPart.quat->u.frame0.value[1] = commonDeltaQuatTrack.m_frames2[0].value[1];
            return;
        }

        const auto numQuatIndices = commonDeltaQuatTrack.m_indices.size();
        const auto indicesArraySize =
            useByteIndices ? numQuatIndices * sizeof(XAnimDynamicIndicesQuat::_1) : numQuatIndices * sizeof(XAnimDynamicIndicesQuat::_2);

        deltaPart.quat = static_cast<XAnimDeltaPartQuat*>(
            memory.AllocRaw(offsetof(XAnimDeltaPartQuat, u) + offsetof(XAnimDeltaPartQuatDataFrames, indices) + indicesArraySize));

        auto& frames = deltaPart.quat->u.frames;
        ConvertIndices(frames.indices, commonDeltaQuatTrack.m_indices, useByteIndices);

        deltaPart.quat->size = static_cast<uint16_t>(numQuatIndices - 1);
        deltaPart.quat->u.frames.frames = memory.Alloc<XQuat2>(numQuatIndices);

        for (auto quatIndexNum = 0u; quatIndexNum < numQuatIndices; ++quatIndexNum)
        {
            const auto& commonFrame = commonDeltaQuatTrack.m_frames2[quatIndexNum];
            auto& curFrame = deltaPart.quat->u.frames.frames[quatIndexNum];

            curFrame.value[0] = commonFrame.value[0];
            curFrame.value[1] = commonFrame.value[1];
        }
    }

    void ConvertCommonDeltaTransPart(XAnimDeltaPart& deltaPart,
                                     const xanim::CommonDeltaTransTrack& commonDeltaTransTrack,
                                     MemoryManager& memory,
                                     const bool useByteIndices)
    {
        if (commonDeltaTransTrack.m_constant)
        {
            deltaPart.trans = static_cast<XAnimPartTrans*>(memory.AllocRaw(offsetof(XAnimPartTrans, u) + sizeof(XAnimPartTransData::frame0)));
            deltaPart.trans->size = 0;
            deltaPart.trans->u.frame0.x = (*commonDeltaTransTrack.m_constant)[0];
            deltaPart.trans->u.frame0.y = (*commonDeltaTransTrack.m_constant)[1];
            deltaPart.trans->u.frame0.z = (*commonDeltaTransTrack.m_constant)[2];
            return;
        }

        const auto numTransIndices = commonDeltaTransTrack.m_indices.size();
        const auto indicesArraySize =
            useByteIndices ? numTransIndices * sizeof(XAnimDynamicIndicesTrans::_1) : numTransIndices * sizeof(XAnimDynamicIndicesTrans::_2);

        deltaPart.trans =
            static_cast<XAnimPartTrans*>(memory.AllocRaw(offsetof(XAnimPartTrans, u) + offsetof(XAnimPartTransFrames, indices) + indicesArraySize));

        auto& frames = deltaPart.trans->u.frames;
        ConvertIndices(frames.indices, commonDeltaTransTrack.m_indices, useByteIndices);

        deltaPart.trans->size = static_cast<uint16_t>(numTransIndices - 1);
        frames.mins.x = commonDeltaTransTrack.m_mins[0];
        frames.mins.y = commonDeltaTransTrack.m_mins[1];
        frames.mins.z = commonDeltaTransTrack.m_mins[2];
        frames.size.x = commonDeltaTransTrack.m_size[0];
        frames.size.y = commonDeltaTransTrack.m_size[1];
        frames.size.z = commonDeltaTransTrack.m_size[2];

        if (commonDeltaTransTrack.m_frames_u16.empty())
        {
            deltaPart.trans->smallTrans = 1;

            static_assert(sizeof(ByteVec) == sizeof(xanim::CommonVec3U8));
            frames.frames._1 = memory.Alloc<ByteVec>(numTransIndices);
            std::memcpy(frames.frames._1, commonDeltaTransTrack.m_frames_u8.data(), numTransIndices * sizeof(ByteVec));
        }
        else
        {
            deltaPart.trans->smallTrans = 0;

            static_assert(sizeof(UShortVec) == sizeof(xanim::CommonVec3U16));
            frames.frames._2 = memory.Alloc<UShortVec>(numTransIndices);
            std::memcpy(frames.frames._2, commonDeltaTransTrack.m_frames_u16.data(), numTransIndices * sizeof(UShortVec));
        }
    }

    void ConvertCommonDeltaPart(XAnimDeltaPart& deltaPart,
                                const xanim::CommonXAnimDeltaTrack& commonXAnimDeltaTrack,
                                MemoryManager& memory,
                                const bool useByteIndices)
    {
        if (commonXAnimDeltaTrack.m_quat)
            ConvertCommonDeltaQuatPart(deltaPart, *commonXAnimDeltaTrack.m_quat, memory, useByteIndices);
        if (commonXAnimDeltaTrack.m_trans)
            ConvertCommonDeltaTransPart(deltaPart, *commonXAnimDeltaTrack.m_trans, memory, useByteIndices);
    }

    void ConvertCommonXAnim(XAnimParts& parts,
                            const xanim::CommonXAnimParts& commonParts,
                            AssetRegistration<AssetXAnim>& registration,
                            MemoryManager& memory,
                            ZoneScriptStrings& scriptStrings)
    {
        parts.numframes = static_cast<decltype(XAnimParts::numframes)>(commonParts.m_num_frames);
        parts.bLoop = commonParts.m_looped;
        parts.assetType = commonParts.m_asset_type;
        parts.framerate = commonParts.m_frame_rate;
        parts.frequency = parts.numframes > 0 ? parts.framerate / static_cast<float>(parts.numframes) : 0;

        const auto useByteIndices = parts.numframes < 256;

        if (commonParts.m_delta_track)
        {
            parts.deltaPart = memory.Alloc<XAnimDeltaPart>();
            parts.bDelta = true;
            ConvertCommonDeltaPart(*parts.deltaPart, *commonParts.m_delta_track, memory, useByteIndices);
        }

        parts.names = memory.Alloc<ScriptString>(commonParts.m_bone_tracks.size());
        for (size_t boneIndex = 0; boneIndex < commonParts.m_bone_tracks.size(); ++boneIndex)
        {
            const auto nameScrString = scriptStrings.AddOrGetScriptString(commonParts.m_bone_tracks[boneIndex].m_name);
            parts.names[boneIndex] = nameScrString;
            registration.AddScriptString(nameScrString);
            CountBoneTrackTypes(parts, commonParts.m_bone_tracks[boneIndex]);
        }
        parts.boneCount[PART_TYPE_ALL] = static_cast<uint8_t>(commonParts.m_bone_tracks.size());

        ConvertNoteTracks(parts, commonParts, registration, memory, scriptStrings);

        const auto flatData = xanim::CreateFlatDataFromCommonXAnim(commonParts);
        ConvertFlatData(parts, flatData, memory);
    }

    class XAnimLoader final : public AssetCreator<AssetXAnim>
    {
    public:
        XAnimLoader(MemoryManager& memory, ISearchPath& searchPath, ZoneScriptStrings& scriptStrings)
            : m_memory(memory),
              m_search_path(searchPath),
              m_script_strings(scriptStrings)
        {
        }

        AssetCreationResult CreateAsset(const std::string& assetName, AssetCreationContext& context) override
        {
            const auto file = m_search_path.Open(xanim::GetCompiledFileNameForAssetName(assetName));
            if (!file.IsOpen())
                return AssetCreationResult::NoAction();

            auto maybeCommonParts = xanim::LoadCompiledXAnim(*file.m_stream);
            if (!maybeCommonParts.has_value())
            {
                con::error("Failed to load xanim \"{}\": {}", assetName, maybeCommonParts.error());
                return AssetCreationResult::Failure();
            }
            const auto commonParts = std::move(maybeCommonParts).value();

            auto* parts = m_memory.Alloc<XAnimParts>();
            parts->name = m_memory.Dup(assetName.c_str());

            AssetRegistration<AssetXAnim> registration(assetName, parts);
            ConvertCommonXAnim(*parts, *commonParts, registration, m_memory, m_script_strings);

            return AssetCreationResult::Success(context.AddAsset(std::move(registration)));
        }

        MemoryManager& m_memory;
        ISearchPath& m_search_path;
        ZoneScriptStrings& m_script_strings;
    };
} // namespace

namespace xanim
{
    std::unique_ptr<AssetCreator<AssetXAnim>> CreateLoaderIW3(MemoryManager& memory, ISearchPath& searchPath, Zone& zone)
    {
        return std::make_unique<XAnimLoader>(memory, searchPath, zone.m_script_strings);
    }
} // namespace xanim
