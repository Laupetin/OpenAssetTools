#include "GltfXAnimWriter.h"

#include "GitVersion.h"
#include "Utils/QuatInt16.h"
#include "XModel/Gltf/GltfConstants.h"
#include "XModel/Gltf/JsonGltf.h"

#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <limits>
#include <nlohmann/json.hpp>
#include <optional>
#include <utility>
#include <vector>

namespace
{
    constexpr auto GLTF_GENERATOR = "OpenAssetTools " GIT_VERSION;
    constexpr auto XANIM_EXTRAS_KEY = "OAT_xanim";
    constexpr auto DELTA_NODE_NAME = "_oat_delta";
    constexpr auto COORDINATE_ROOT_NODE_NAME = "_oat_xanim";
    constexpr auto NEGATIVE_SQRT_HALF = -0.7071067811865475244f;
    constexpr auto POSITIVE_SQRT_HALF = 0.7071067811865475244f;

    template<size_t N> using FloatValues = std::vector<std::array<float, N>>;

    void NormalizeQuaternion(std::array<float, 4>& quat)
    {
        const auto length = std::sqrt(quat[0] * quat[0] + quat[1] * quat[1] + quat[2] * quat[2] + quat[3] * quat[3]);
        if (length <= std::numeric_limits<float>::epsilon())
        {
            quat = {0.0f, 0.0f, 0.0f, 1.0f};
            return;
        }

        for (auto& component : quat)
            component /= length;
    }

    std::vector<float> CreateTimes(const std::vector<uint16_t>& indices, const float frameRate)
    {
        std::vector<float> result;
        result.reserve(indices.size());
        for (const auto index : indices)
            result.emplace_back(static_cast<float>(index) / frameRate);
        return result;
    }

    std::array<float, 3> DecodeTranslation(const xanim::TransTrack& track, const size_t frameIndex)
    {
        std::array<float, 3> result{};
        if (track.m_type == xanim::TransType::TRANS_NO_SIZE)
            result = track.m_constant;
        else if (track.m_type == xanim::TransType::SMALL_TRANS)
        {
            for (auto component = 0u; component < 3u; component++)
                result[component] = track.m_mins[component] + track.m_size[component] * static_cast<float>(track.m_frames_u8[frameIndex].value[component]);
        }
        else
        {
            for (auto component = 0u; component < 3u; component++)
                result[component] = track.m_mins[component] + track.m_size[component] * static_cast<float>(track.m_frames_u16[frameIndex].value[component]);
        }

        return result;
    }

    std::array<float, 3> DecodeTranslation(const xanim::CommonDeltaTransTrack& track, const size_t frameIndex)
    {
        std::array<float, 3> result{};
        if (track.m_constant)
            result = *track.m_constant;
        else if (!track.m_frames_u8.empty())
        {
            for (auto component = 0u; component < 3u; component++)
                result[component] = track.m_mins[component] + track.m_size[component] * static_cast<float>(track.m_frames_u8[frameIndex].value[component]);
        }
        else
        {
            for (auto component = 0u; component < 3u; component++)
                result[component] = track.m_mins[component] + track.m_size[component] * static_cast<float>(track.m_frames_u16[frameIndex].value[component]);
        }

        return result;
    }

    std::array<float, 4> DecodeQuaternion(const xanim::CommonXQuat& quat)
    {
        std::array<float, 4> result{
            QuatInt16::ToFloat(quat.value[0]),
            QuatInt16::ToFloat(quat.value[1]),
            QuatInt16::ToFloat(quat.value[2]),
            QuatInt16::ToFloat(quat.value[3]),
        };
        NormalizeQuaternion(result);
        return result;
    }

    std::array<float, 4> DecodeQuaternion(const xanim::CommonXQuat2& quat)
    {
        std::array<float, 4> result{0.0f, 0.0f, QuatInt16::ToFloat(quat.value[0]), QuatInt16::ToFloat(quat.value[1])};
        NormalizeQuaternion(result);
        return result;
    }

    class Writer
    {
    public:
        Writer(const xanim::CommonXAnimParts& parts, std::string name, const gltf::Output& output)
            : m_parts(parts),
              m_name(std::move(name)),
              m_output(output),
              m_frame_rate(parts.m_frame_rate > 0.0f ? parts.m_frame_rate : 30.0f)
        {
        }

        void Write()
        {
            gltf::JsonRoot root;
            root.asset.version = gltf::GLTF_VERSION_STRING;
            root.asset.generator = GLTF_GENERATOR;
            root.nodes.emplace();
            root.animations.emplace(1u);
            root.animations->front().name = m_name;

            gltf::JsonScene scene;
            gltf::JsonNode coordinateRoot;
            coordinateRoot.name = COORDINATE_ROOT_NODE_NAME;
            coordinateRoot.rotation = std::array<float, 4>{NEGATIVE_SQRT_HALF, 0.0f, 0.0f, POSITIVE_SQRT_HALF};
            coordinateRoot.children.emplace();
            scene.nodes.emplace_back(0u);
            root.nodes->emplace_back(std::move(coordinateRoot));

            for (const auto& bone : m_parts.m_bone_tracks)
            {
                gltf::JsonNode node;
                node.name = bone.m_name;
                root.nodes->front().children->emplace_back(static_cast<unsigned>(root.nodes->size()));
                root.nodes->emplace_back(std::move(node));
            }

            auto& animation = root.animations->front();
            for (auto boneIndex = 0uz; boneIndex < m_parts.m_bone_tracks.size(); boneIndex++)
                AddBoneChannels(root, animation, static_cast<unsigned>(boneIndex + 1uz), m_parts.m_bone_tracks[boneIndex]);

            std::optional<unsigned> deltaNode;
            if (m_parts.m_delta_track)
            {
                gltf::JsonNode node;
                node.name = DELTA_NODE_NAME;
                deltaNode = static_cast<unsigned>(root.nodes->size());
                root.nodes->front().children->emplace_back(*deltaNode);
                root.nodes->emplace_back(std::move(node));
                AddDeltaChannels(root, animation, *deltaNode, *m_parts.m_delta_track);
            }

            root.scenes.emplace(1u, std::move(scene));
            root.scene = 0u;

            if (!m_buffer.empty())
            {
                root.buffers.emplace();
                gltf::JsonBuffer buffer;
                buffer.byteLength = static_cast<unsigned>(m_buffer.size());
                buffer.uri = m_output.CreateBufferUri(m_buffer.data(), m_buffer.size());
                root.buffers->emplace_back(std::move(buffer));
            }

            nlohmann::ordered_json jsonRoot = root;
            auto& metadata = jsonRoot["extras"][XANIM_EXTRAS_KEY];
            metadata["version"] = 1u;
            metadata["numFrames"] = m_parts.m_num_frames;
            metadata["frameRate"] = m_frame_rate;
            metadata["looped"] = m_parts.m_looped;
            metadata["assetType"] = m_parts.m_asset_type;
            metadata["boneNodes"] = nlohmann::ordered_json::array();
            for (auto boneIndex = 0uz; boneIndex < m_parts.m_bone_tracks.size(); boneIndex++)
                metadata["boneNodes"].emplace_back(boneIndex + 1uz);
            metadata["notifies"] = nlohmann::ordered_json::array();
            for (const auto& notify : m_parts.m_notifies)
                metadata["notifies"].emplace_back(nlohmann::ordered_json{
                    {"name", notify.m_name},
                    {"time", notify.m_time}
                });

            if (deltaNode)
            {
                metadata["deltaNode"] = *deltaNode;
                metadata["delta3D"] = m_parts.m_delta_track->m_quat && m_parts.m_delta_track->m_quat->Is3DTrack();
            }

            m_output.EmitJson(jsonRoot);
            if (!m_buffer.empty())
                m_output.EmitBuffer(m_buffer.data(), m_buffer.size());
            m_output.Finalize();
        }

    private:
        unsigned AddAccessor(gltf::JsonRoot& root,
                             const void* data,
                             const size_t byteLength,
                             const size_t count,
                             const gltf::JsonAccessorType type,
                             std::optional<std::vector<float>> min = std::nullopt,
                             std::optional<std::vector<float>> max = std::nullopt)
        {
            if (!root.bufferViews)
                root.bufferViews.emplace();
            if (!root.accessors)
                root.accessors.emplace();

            const auto offset = m_buffer.size();
            const auto* bytes = static_cast<const uint8_t*>(data);
            m_buffer.insert(m_buffer.end(), bytes, bytes + byteLength);

            gltf::JsonBufferView view;
            view.buffer = 0u;
            view.byteOffset = static_cast<unsigned>(offset);
            view.byteLength = static_cast<unsigned>(byteLength);
            const auto viewIndex = static_cast<unsigned>(root.bufferViews->size());
            root.bufferViews->emplace_back(std::move(view));

            gltf::JsonAccessor accessor;
            accessor.bufferView = viewIndex;
            accessor.componentType = gltf::JsonAccessorComponentType::FLOAT;
            accessor.count = static_cast<unsigned>(count);
            accessor.type = type;
            accessor.min = std::move(min);
            accessor.max = std::move(max);
            const auto accessorIndex = static_cast<unsigned>(root.accessors->size());
            root.accessors->emplace_back(std::move(accessor));
            return accessorIndex;
        }

        unsigned AddTimes(gltf::JsonRoot& root, const std::vector<float>& times)
        {
            assert(!times.empty());
            const auto [min, max] = std::ranges::minmax_element(times);
            return AddAccessor(root,
                               times.data(),
                               times.size() * sizeof(float),
                               times.size(),
                               gltf::JsonAccessorType::SCALAR,
                               std::vector<float>{*min},
                               std::vector<float>{*max});
        }

        template<size_t N> unsigned AddValues(gltf::JsonRoot& root, const FloatValues<N>& values, const gltf::JsonAccessorType type)
        {
            assert(!values.empty());
            return AddAccessor(root, values.data(), values.size() * sizeof(std::array<float, N>), values.size(), type);
        }

        static void AddChannel(
            gltf::JsonAnimation& animation, const unsigned node, const gltf::JsonAnimationChannelTargetPath path, const unsigned input, const unsigned output)
        {
            gltf::JsonAnimationSampler sampler;
            sampler.input = input;
            sampler.output = output;
            sampler.interpolation = gltf::JsonAnimationSamplerInterpolation::LINEAR;
            const auto samplerIndex = static_cast<unsigned>(animation.samplers.size());
            animation.samplers.emplace_back(std::move(sampler));

            gltf::JsonAnimationChannel channel;
            channel.sampler = samplerIndex;
            channel.target.node = node;
            channel.target.path = path;
            animation.channels.emplace_back(std::move(channel));
        }

        void AddBoneChannels(gltf::JsonRoot& root, gltf::JsonAnimation& animation, const unsigned node, const xanim::BoneTrack& bone)
        {
            if (bone.m_quat.m_type != xanim::QuatType::NO_QUAT)
            {
                std::vector<float> times;
                FloatValues<4> values;
                if (bone.m_quat.m_type == xanim::QuatType::HALF_QUAT_NO_SIZE)
                {
                    times = {0.0f};
                    values.emplace_back(DecodeQuaternion(bone.m_quat.m_frames2.front()));
                }
                else if (bone.m_quat.m_type == xanim::QuatType::FULL_QUAT_NO_SIZE)
                {
                    times = {0.0f};
                    values.emplace_back(DecodeQuaternion(bone.m_quat.m_frames.front()));
                }
                else if (bone.m_quat.m_type == xanim::QuatType::HALF_QUAT)
                {
                    times = CreateTimes(bone.m_quat.m_indices, m_frame_rate);
                    for (const auto& frame : bone.m_quat.m_frames2)
                        values.emplace_back(DecodeQuaternion(frame));
                }
                else
                {
                    times = CreateTimes(bone.m_quat.m_indices, m_frame_rate);
                    for (const auto& frame : bone.m_quat.m_frames)
                        values.emplace_back(DecodeQuaternion(frame));
                }

                AddChannel(animation,
                           node,
                           gltf::JsonAnimationChannelTargetPath::ROTATION,
                           AddTimes(root, times),
                           AddValues(root, values, gltf::JsonAccessorType::VEC4));
            }

            if (bone.m_trans.m_type != xanim::TransType::NO_TRANS)
            {
                const auto constant = bone.m_trans.m_type == xanim::TransType::TRANS_NO_SIZE;
                auto times = constant ? std::vector<float>{0.0f} : CreateTimes(bone.m_trans.m_indices, m_frame_rate);
                FloatValues<3> values;
                const auto frameCount = constant ? 1uz : bone.m_trans.m_indices.size();
                for (auto frameIndex = 0uz; frameIndex < frameCount; frameIndex++)
                    values.emplace_back(DecodeTranslation(bone.m_trans, frameIndex));

                AddChannel(animation,
                           node,
                           gltf::JsonAnimationChannelTargetPath::TRANSLATION,
                           AddTimes(root, times),
                           AddValues(root, values, gltf::JsonAccessorType::VEC3));
            }
        }

        void AddDeltaChannels(gltf::JsonRoot& root, gltf::JsonAnimation& animation, const unsigned node, const xanim::CommonXAnimDeltaTrack& delta)
        {
            if (delta.m_quat)
            {
                const auto constant = delta.m_quat->m_indices.empty();
                auto times = constant ? std::vector<float>{0.0f} : CreateTimes(delta.m_quat->m_indices, m_frame_rate);
                FloatValues<4> values;
                if (delta.m_quat->Is3DTrack())
                {
                    for (const auto& frame : delta.m_quat->m_frames)
                        values.emplace_back(DecodeQuaternion(frame));
                }
                else
                {
                    for (const auto& frame : delta.m_quat->m_frames2)
                        values.emplace_back(DecodeQuaternion(frame));
                }

                AddChannel(animation,
                           node,
                           gltf::JsonAnimationChannelTargetPath::ROTATION,
                           AddTimes(root, times),
                           AddValues(root, values, gltf::JsonAccessorType::VEC4));
            }

            if (delta.m_trans)
            {
                const auto constant = delta.m_trans->m_constant.has_value();
                auto times = constant ? std::vector<float>{0.0f} : CreateTimes(delta.m_trans->m_indices, m_frame_rate);
                FloatValues<3> values;
                const auto frameCount = constant ? 1uz : delta.m_trans->m_indices.size();
                for (auto frameIndex = 0uz; frameIndex < frameCount; frameIndex++)
                    values.emplace_back(DecodeTranslation(*delta.m_trans, frameIndex));

                AddChannel(animation,
                           node,
                           gltf::JsonAnimationChannelTargetPath::TRANSLATION,
                           AddTimes(root, times),
                           AddValues(root, values, gltf::JsonAccessorType::VEC3));
            }
        }

        const xanim::CommonXAnimParts& m_parts;
        std::string m_name;
        const gltf::Output& m_output;
        float m_frame_rate;
        std::vector<uint8_t> m_buffer;
    };
} // namespace

namespace xanim_gltf
{
    void Write(const xanim::CommonXAnimParts& parts, const std::string& name, const gltf::Output& output)
    {
        Writer(parts, name, output).Write();
    }
} // namespace xanim_gltf
