#include "GltfXAnimLoader.h"

#include "Utils/QuatInt16.h"
#include "XModel/Gltf/Internal/GltfBuffer.h"
#include "XModel/Gltf/Internal/GltfBufferView.h"
#include "XModel/Gltf/JsonGltf.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <format>
#include <limits>
#include <map>
#include <nlohmann/json.hpp>
#include <optional>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace
{
    constexpr auto XANIM_EXTRAS_KEY = "OAT_xanim";
    constexpr auto DEFAULT_FRAME_RATE = 30.0f;
    constexpr auto EQUALITY_EPSILON = 0.00001f;

    class LoadException final : public std::runtime_error
    {
    public:
        using std::runtime_error::runtime_error;
    };

    template<size_t N> using FloatValues = std::vector<std::array<float, N>>;

    bool NearlyEqual(const float lhs, const float rhs)
    {
        return std::abs(lhs - rhs) <= EQUALITY_EPSILON;
    }

    template<size_t N> bool AllValuesEqual(const FloatValues<N>& values)
    {
        if (values.empty())
            return true;

        for (auto valueIndex = 1uz; valueIndex < values.size(); valueIndex++)
        {
            for (auto component = 0uz; component < N; component++)
            {
                if (!NearlyEqual(values.front()[component], values[valueIndex][component]))
                    return false;
            }
        }

        return true;
    }

    class Loader
    {
    public:
        explicit Loader(const gltf::Input& input)
            : m_input(input)
        {
        }

        std::unique_ptr<xanim::CommonXAnimParts> Load()
        {
            gltf::JsonRoot root;
            try
            {
                root = m_input.GetJson().get<gltf::JsonRoot>();
            }
            catch (const nlohmann::json::exception& e)
            {
                throw LoadException(std::format("Failed to parse GLTF JSON: {}", e.what()));
            }

            if (!root.animations || root.animations->empty())
                throw LoadException("GLTF does not contain an animation");
            if (!root.nodes)
                throw LoadException("GLTF animation does not contain nodes");

            CreateBuffers(root);
            CreateBufferViews(root);

            const auto* metadata = FindMetadata();
            ReadMetadata(metadata);

            std::map<unsigned, xanim::BoneTrack> boneTracks;
            if (metadata && metadata->contains("boneNodes"))
            {
                for (const auto& boneNodeJson : metadata->at("boneNodes"))
                {
                    const auto nodeIndex = boneNodeJson.get<unsigned>();
                    if (!boneTracks.emplace(nodeIndex, CreateBoneTrackForNode(root, nodeIndex)).second)
                        throw LoadException(std::format("XAnim metadata contains duplicate bone node {}", nodeIndex));
                }
            }

            std::optional<xanim::BoneTrack> deltaTrack;
            if (m_delta_node)
            {
                if (boneTracks.contains(*m_delta_node))
                    throw LoadException("XAnim metadata uses its delta node as a bone node");
                deltaTrack = CreateBoneTrackForNode(root, *m_delta_node);
            }

            bool hasTransformChannels = false;
            const auto& animation = root.animations->front();
            for (const auto& channel : animation.channels)
            {
                if (channel.sampler >= animation.samplers.size())
                    throw LoadException("Animation channel references an invalid sampler");
                if (channel.target.node >= root.nodes->size())
                    throw LoadException("Animation channel references an invalid node");

                const auto& sampler = animation.samplers[channel.sampler];
                if (sampler.interpolation && *sampler.interpolation != gltf::JsonAnimationSamplerInterpolation::LINEAR)
                    throw LoadException("Only LINEAR animation channels are supported");

                const auto times = ReadFloatValues<1>(root, sampler.input, gltf::JsonAccessorType::SCALAR);
                std::vector<float> scalarTimes;
                scalarTimes.reserve(times.size());
                for (const auto& time : times)
                {
                    if (!std::isfinite(time[0]) || time[0] < 0.0f)
                        throw LoadException("Animation contains an invalid keyframe time");
                    scalarTimes.emplace_back(time[0]);
                    m_max_time = std::max(m_max_time, time[0]);
                }
                auto frameIndices = CreateFrameIndices(scalarTimes);

                if (channel.target.path == gltf::JsonAnimationChannelTargetPath::SCALE)
                {
                    const auto values = ReadFloatValues<3>(root, sampler.output, gltf::JsonAccessorType::VEC3);
                    ValidateScaleTrack(scalarTimes, values);
                    continue;
                }
                if (channel.target.path == gltf::JsonAnimationChannelTargetPath::WEIGHTS)
                    continue;

                xanim::BoneTrack* track;
                if (m_delta_node && channel.target.node == *m_delta_node)
                    track = &*deltaTrack;
                else
                    track = &GetOrCreateBoneTrack(root, boneTracks, channel.target.node);

                if (channel.target.path == gltf::JsonAnimationChannelTargetPath::ROTATION)
                {
                    if (track->m_quat.m_type != xanim::QuatType::NO_QUAT)
                        throw LoadException(std::format("Node {} has multiple rotation channels", channel.target.node));
                    const auto values = ReadFloatValues<4>(root, sampler.output, gltf::JsonAccessorType::VEC4);
                    ConvertRotationTrack(track->m_quat, std::move(frameIndices), values);
                    hasTransformChannels = true;
                }
                else if (channel.target.path == gltf::JsonAnimationChannelTargetPath::TRANSLATION)
                {
                    if (track->m_trans.m_type != xanim::TransType::NO_TRANS)
                        throw LoadException(std::format("Node {} has multiple translation channels", channel.target.node));
                    const auto values = ReadFloatValues<3>(root, sampler.output, gltf::JsonAccessorType::VEC3);
                    ConvertTranslationTrack(track->m_trans, std::move(frameIndices), values);
                    hasTransformChannels = true;
                }
            }

            if (!metadata && !hasTransformChannels)
                throw LoadException("GLTF animation does not contain bone translation or rotation channels");
            if (boneTracks.size() > std::numeric_limits<uint8_t>::max())
                throw LoadException("Animation has too many bone nodes for an XAnim");

            auto result = std::make_unique<xanim::CommonXAnimParts>();
            result->m_frame_rate = m_frame_rate;
            result->m_num_frames = m_num_frames.value_or(static_cast<size_t>(std::lround(m_max_time * m_frame_rate)));
            if (result->m_num_frames > std::numeric_limits<uint16_t>::max() - 1uz)
                throw LoadException("Animation has too many frames for an XAnim");
            if (m_max_frame > result->m_num_frames)
                throw LoadException("Animation metadata numFrames is shorter than its animation channels");

            result->m_looped = m_looped;
            result->m_asset_type = m_asset_type;
            result->m_notifies = std::move(m_notifies);
            result->m_bone_tracks.reserve(boneTracks.size());
            for (auto& entry : boneTracks)
                result->m_bone_tracks.emplace_back(std::move(entry.second));
            result->SortBoneTracksForQuats();

            if (deltaTrack)
                result->m_delta_track = ConvertDeltaTrack(std::move(*deltaTrack));

            return result;
        }

    private:
        const nlohmann::json* FindMetadata() const
        {
            const auto& root = m_input.GetJson();
            const auto extras = root.find("extras");
            if (extras == root.end() || !extras->is_object())
                return nullptr;

            const auto metadata = extras->find(XANIM_EXTRAS_KEY);
            if (metadata == extras->end() || !metadata->is_object())
                return nullptr;
            return &*metadata;
        }

        void ReadMetadata(const nlohmann::json* metadata)
        {
            if (!metadata)
                return;

            m_frame_rate = metadata->value("frameRate", DEFAULT_FRAME_RATE);
            if (!std::isfinite(m_frame_rate) || m_frame_rate <= 0.0f)
                throw LoadException("XAnim frameRate metadata must be greater than zero");

            if (metadata->contains("numFrames"))
                m_num_frames = metadata->at("numFrames").get<size_t>();
            m_looped = metadata->value("looped", false);
            m_asset_type = metadata->value("assetType", static_cast<uint8_t>(0u));
            if (metadata->contains("deltaNode"))
                m_delta_node = metadata->at("deltaNode").get<unsigned>();
            m_delta_3d = metadata->value("delta3D", true);

            if (metadata->contains("notifies"))
            {
                for (const auto& notifyJson : metadata->at("notifies"))
                {
                    const auto name = notifyJson.at("name").get<std::string>();
                    const auto time = notifyJson.at("time").get<float>();
                    if (!std::isfinite(time) || time < 0.0f || time > 1.0f)
                        throw LoadException(std::format("XAnim notify {} has an invalid normalized time", name));
                    m_notifies.emplace_back(name, time);
                }
            }
        }

        void CreateBuffers(const gltf::JsonRoot& root)
        {
            if (!root.buffers)
                return;

            m_buffers.reserve(root.buffers->size());
            for (const auto& jsonBuffer : *root.buffers)
            {
                if (!jsonBuffer.uri)
                {
                    const void* data = nullptr;
                    size_t dataSize = 0u;
                    if (!m_input.GetEmbeddedBuffer(data, dataSize) || dataSize == 0u)
                        throw LoadException("GLTF references a missing GLB buffer");
                    m_buffers.emplace_back(std::make_unique<gltf::EmbeddedBuffer>(data, dataSize));
                }
                else if (gltf::DataUriBuffer::IsDataUri(*jsonBuffer.uri))
                {
                    auto buffer = std::make_unique<gltf::DataUriBuffer>();
                    if (!buffer->ReadDataFromUri(*jsonBuffer.uri))
                        throw LoadException("GLTF contains an invalid data URI buffer");
                    m_buffers.emplace_back(std::move(buffer));
                }
                else
                {
                    throw LoadException("External GLTF buffer files are not supported; export embedded GLTF or GLB");
                }
            }
        }

        void CreateBufferViews(const gltf::JsonRoot& root)
        {
            if (!root.bufferViews)
                return;

            m_buffer_views.reserve(root.bufferViews->size());
            for (const auto& jsonView : *root.bufferViews)
            {
                if (jsonView.buffer >= m_buffers.size())
                    throw LoadException("Buffer view references an invalid buffer");
                const auto* buffer = m_buffers[jsonView.buffer].get();
                const auto offset = jsonView.byteOffset.value_or(0u);
                if (offset + jsonView.byteLength > buffer->GetSize())
                    throw LoadException("Buffer view exceeds its underlying buffer");

                m_buffer_views.emplace_back(std::make_unique<gltf::BufferView>(buffer, offset, jsonView.byteLength, jsonView.byteStride.value_or(0u)));
            }
        }

        template<size_t N>
        FloatValues<N> ReadFloatValues(const gltf::JsonRoot& root, const unsigned accessorIndex, const gltf::JsonAccessorType expectedType) const
        {
            if (!root.accessors || accessorIndex >= root.accessors->size())
                throw LoadException("Animation sampler references an invalid accessor");
            const auto& accessor = (*root.accessors)[accessorIndex];
            if (accessor.componentType != gltf::JsonAccessorComponentType::FLOAT || accessor.type != expectedType)
                throw LoadException("Animation accessor must use floating point values of the expected shape");
            if (!accessor.bufferView || *accessor.bufferView >= m_buffer_views.size())
                throw LoadException("Sparse and empty animation accessors are not supported");

            FloatValues<N> result(accessor.count);
            const auto* view = m_buffer_views[*accessor.bufferView].get();
            const auto byteOffset = accessor.byteOffset.value_or(0u);
            for (auto valueIndex = 0uz; valueIndex < result.size(); valueIndex++)
            {
                if (!view->ReadElement(result[valueIndex].data(), valueIndex, sizeof(float) * N, byteOffset))
                    throw LoadException("Animation accessor exceeds its buffer view");
            }
            return result;
        }

        static xanim::BoneTrack CreateBoneTrackForNode(const gltf::JsonRoot& root, const unsigned nodeIndex)
        {
            if (!root.nodes || nodeIndex >= root.nodes->size())
                throw LoadException("XAnim metadata references an invalid bone node");
            const auto& node = (*root.nodes)[nodeIndex];
            if (!node.name || node.name->empty())
                throw LoadException(std::format("Animated node {} must have a bone name", nodeIndex));

            xanim::BoneTrack result;
            result.m_name = *node.name;
            return result;
        }

        static xanim::BoneTrack& GetOrCreateBoneTrack(const gltf::JsonRoot& root, std::map<unsigned, xanim::BoneTrack>& tracks, const unsigned nodeIndex)
        {
            const auto existing = tracks.find(nodeIndex);
            if (existing != tracks.end())
                return existing->second;
            return tracks.emplace(nodeIndex, CreateBoneTrackForNode(root, nodeIndex)).first->second;
        }

        std::vector<uint16_t> CreateFrameIndices(const std::vector<float>& times)
        {
            std::vector<uint16_t> result;
            result.reserve(times.size());
            for (const auto time : times)
            {
                const auto frame = std::lround(time * m_frame_rate);
                if (frame < 0 || frame > std::numeric_limits<uint16_t>::max())
                    throw LoadException("Animation keyframe lies outside the XAnim frame range");
                if (!result.empty() && frame <= result.back())
                    throw LoadException("Animation keyframes must map to distinct, increasing XAnim frames");
                result.emplace_back(static_cast<uint16_t>(frame));
                m_max_frame = std::max(m_max_frame, static_cast<size_t>(frame));
            }
            return result;
        }

        static void ValidateScaleTrack(const std::vector<float>& times, const FloatValues<3>& values)
        {
            if (times.empty() || times.size() != values.size())
                throw LoadException("Scale sampler input and output counts do not match");

            for (const auto& value : values)
            {
                if (!std::ranges::all_of(value,
                                         [](const float component)
                                         {
                                             return std::isfinite(component) && NearlyEqual(component, 1.0f);
                                         }))
                    throw LoadException("Animated or non-identity bone scale is not supported by XAnims");
            }
        }

        static void ConvertRotationTrack(xanim::QuatTrack& track, std::vector<uint16_t> frameIndices, FloatValues<4> values)
        {
            if (frameIndices.empty() || frameIndices.size() != values.size())
                throw LoadException("Rotation sampler input and output counts do not match");

            track.m_frames.reserve(values.size());
            for (auto valueIndex = 0uz; valueIndex < values.size(); valueIndex++)
            {
                auto& value = values[valueIndex];
                const auto length = std::sqrt(value[0] * value[0] + value[1] * value[1] + value[2] * value[2] + value[3] * value[3]);
                if (!std::isfinite(length) || length <= std::numeric_limits<float>::epsilon())
                    throw LoadException("Animation contains an invalid zero-length quaternion");
                for (auto& component : value)
                    component /= length;

                if (valueIndex > 0uz)
                {
                    const auto& previous = values[valueIndex - 1uz];
                    const auto dot = previous[0] * value[0] + previous[1] * value[1] + previous[2] * value[2] + previous[3] * value[3];
                    if (dot < 0.0f)
                    {
                        for (auto& component : value)
                            component = -component;
                    }
                }

                track.m_frames.emplace_back(QuatInt16::ToInt16(std::clamp(value[0], -1.0f, 1.0f)),
                                            QuatInt16::ToInt16(std::clamp(value[1], -1.0f, 1.0f)),
                                            QuatInt16::ToInt16(std::clamp(value[2], -1.0f, 1.0f)),
                                            QuatInt16::ToInt16(std::clamp(value[3], -1.0f, 1.0f)));
            }

            if (values.size() == 1uz)
                track.m_type = xanim::QuatType::FULL_QUAT_NO_SIZE;
            else
            {
                track.m_type = xanim::QuatType::FULL_QUAT;
                track.m_indices = std::move(frameIndices);
            }
        }

        static void ConvertTranslationTrack(xanim::TransTrack& track, std::vector<uint16_t> frameIndices, FloatValues<3> values)
        {
            if (frameIndices.empty() || frameIndices.size() != values.size())
                throw LoadException("Translation sampler input and output counts do not match");
            for (auto& value : values)
            {
                if (!std::ranges::all_of(value,
                                         [](const float component)
                                         {
                                             return std::isfinite(component);
                                         }))
                    throw LoadException("Animation contains an invalid translation");
            }

            if (values.size() == 1uz || AllValuesEqual(values))
            {
                track.m_type = xanim::TransType::TRANS_NO_SIZE;
                track.m_constant = values.front();
                return;
            }

            track.m_type = xanim::TransType::FULL_TRANS;
            track.m_indices = std::move(frameIndices);
            for (auto component = 0uz; component < 3uz; component++)
            {
                const auto [min, max] = std::ranges::minmax_element(values,
                                                                    {},
                                                                    [component](const auto& value)
                                                                    {
                                                                        return value[component];
                                                                    });
                track.m_mins[component] = (*min)[component];
                track.m_size[component] = ((*max)[component] - (*min)[component]) / static_cast<float>(std::numeric_limits<uint16_t>::max());
            }

            track.m_frames_u16.reserve(values.size());
            for (const auto& value : values)
            {
                std::array<uint16_t, 3> encoded{};
                for (auto component = 0uz; component < 3uz; component++)
                {
                    if (track.m_size[component] > 0.0f)
                    {
                        encoded[component] =
                            static_cast<uint16_t>(std::clamp(std::lround((value[component] - track.m_mins[component]) / track.m_size[component]), 0l, 65535l));
                    }
                }
                track.m_frames_u16.emplace_back(encoded[0], encoded[1], encoded[2]);
            }
        }

        std::unique_ptr<xanim::CommonXAnimDeltaTrack> ConvertDeltaTrack(xanim::BoneTrack track) const
        {
            auto result = std::make_unique<xanim::CommonXAnimDeltaTrack>();
            if (track.m_quat.m_type != xanim::QuatType::NO_QUAT)
            {
                xanim::CommonDeltaQuatTrack quat;
                quat.m_indices = std::move(track.m_quat.m_indices);
                if (m_delta_3d)
                    quat.m_frames = std::move(track.m_quat.m_frames);
                else
                {
                    quat.m_frames2.reserve(track.m_quat.m_frames.size());
                    for (const auto& frame : track.m_quat.m_frames)
                        quat.m_frames2.emplace_back(frame.value[2], frame.value[3]);
                }
                result->m_quat = std::move(quat);
            }

            if (track.m_trans.m_type != xanim::TransType::NO_TRANS)
            {
                xanim::CommonDeltaTransTrack trans;
                if (track.m_trans.m_type == xanim::TransType::TRANS_NO_SIZE)
                    trans.m_constant = track.m_trans.m_constant;
                else
                {
                    trans.m_indices = std::move(track.m_trans.m_indices);
                    trans.m_mins = track.m_trans.m_mins;
                    trans.m_size = track.m_trans.m_size;
                    trans.m_frames_u16 = std::move(track.m_trans.m_frames_u16);
                }
                result->m_trans = std::move(trans);
            }
            return result;
        }

        const gltf::Input& m_input;
        std::vector<std::unique_ptr<gltf::Buffer>> m_buffers;
        std::vector<std::unique_ptr<gltf::BufferView>> m_buffer_views;
        float m_frame_rate = DEFAULT_FRAME_RATE;
        float m_max_time = 0.0f;
        size_t m_max_frame = 0uz;
        std::optional<size_t> m_num_frames;
        bool m_looped = false;
        uint8_t m_asset_type = 0u;
        std::optional<unsigned> m_delta_node;
        bool m_delta_3d = true;
        std::vector<xanim::CommonXAnimNotifyInfo> m_notifies;
    };
} // namespace

namespace xanim_gltf
{
    std::expected<std::unique_ptr<xanim::CommonXAnimParts>, std::string> Load(const gltf::Input& input)
    {
        try
        {
            return Loader(input).Load();
        }
        catch (const LoadException& e)
        {
            return std::unexpected(e.what());
        }
        catch (const nlohmann::json::exception& e)
        {
            return std::unexpected(std::format("Invalid XAnim GLTF metadata: {}", e.what()));
        }
    }
} // namespace xanim_gltf
