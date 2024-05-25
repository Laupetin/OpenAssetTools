#pragma once

#include "Json/JsonExtension.h"
#include <array>
#include <nlohmann/json.hpp>
#include <optional>
#include <string>
#include <vector>

namespace gltf
{
    class JsonAsset
    {
    public:
        std::string version;
        std::optional<std::string> generator;
    };

    NLOHMANN_DEFINE_TYPE_EXTENSION(JsonAsset, version, generator);

    class JsonNode
    {
    public:
        std::optional<std::string> name;
        std::optional<std::array<float, 3>> translation;
        std::optional<std::array<float, 4>> rotation;
        std::optional<std::array<float, 3>> scale;
        std::optional<std::vector<unsigned>> children;
        std::optional<unsigned> skin;
        std::optional<unsigned> mesh;
    };

    NLOHMANN_DEFINE_TYPE_EXTENSION(JsonNode, name, translation, rotation, scale, children, skin, mesh);

    class JsonBuffer
    {
    public:
        unsigned byteLength;
        std::optional<std::string> uri;
    };

    NLOHMANN_DEFINE_TYPE_EXTENSION(JsonBuffer, byteLength, uri);

    enum class JsonAccessorComponentType
    {
        SIGNED_BYTE = 5120,
        UNSIGNED_BYTE = 5121,
        SIGNED_SHORT = 5122,
        UNSIGNED_SHORT = 5123,
        UNSIGNED_INT = 5125,
        FLOAT = 5126
    };

    NLOHMANN_JSON_SERIALIZE_ENUM(JsonAccessorComponentType,
                                 {
                                     {JsonAccessorComponentType::SIGNED_BYTE,    static_cast<unsigned>(JsonAccessorComponentType::SIGNED_BYTE)   },
                                     {JsonAccessorComponentType::UNSIGNED_BYTE,  static_cast<unsigned>(JsonAccessorComponentType::UNSIGNED_BYTE) },
                                     {JsonAccessorComponentType::SIGNED_SHORT,   static_cast<unsigned>(JsonAccessorComponentType::SIGNED_SHORT)  },
                                     {JsonAccessorComponentType::UNSIGNED_SHORT, static_cast<unsigned>(JsonAccessorComponentType::UNSIGNED_SHORT)},
                                     {JsonAccessorComponentType::UNSIGNED_INT,   static_cast<unsigned>(JsonAccessorComponentType::UNSIGNED_INT)  },
                                     {JsonAccessorComponentType::FLOAT,          static_cast<unsigned>(JsonAccessorComponentType::FLOAT)         },
    });

    enum class JsonAccessorType
    {
        SCALAR,
        VEC2,
        VEC3,
        VEC4,
        MAT2,
        MAT3,
        MAT4
    };

    NLOHMANN_JSON_SERIALIZE_ENUM(JsonAccessorType,
                                 {
                                     {JsonAccessorType::SCALAR, "SCALAR"},
                                     {JsonAccessorType::VEC2,   "VEC2"  },
                                     {JsonAccessorType::VEC3,   "VEC3"  },
                                     {JsonAccessorType::VEC4,   "VEC4"  },
                                     {JsonAccessorType::MAT2,   "MAT2"  },
                                     {JsonAccessorType::MAT3,   "MAT3"  },
                                     {JsonAccessorType::MAT4,   "MAT4"  },
    });

    class JsonAccessor
    {
    public:
        std::optional<unsigned> bufferView;
        std::optional<unsigned> byteOffset;
        JsonAccessorComponentType componentType;
        // std::optional<boolean> normalized
        unsigned count;
        JsonAccessorType type;
        std::optional<std::vector<float>> max;
        std::optional<std::vector<float>> min;
        // std::optional<JsonAccessorSparse> sparse;
        // std::optional<std::string> name;
        // extensions
        // extras
    };

    NLOHMANN_DEFINE_TYPE_EXTENSION(JsonAccessor, bufferView, byteOffset, componentType, count, type, min, max);

    enum class JsonBufferViewTarget
    {
        ARRAY_BUFFER = 34962,
        ELEMENT_ARRAY_BUFFER = 34963
    };

    NLOHMANN_JSON_SERIALIZE_ENUM(JsonBufferViewTarget,
                                 {
                                     {JsonBufferViewTarget::ARRAY_BUFFER,         static_cast<unsigned>(JsonBufferViewTarget::ARRAY_BUFFER)        },
                                     {JsonBufferViewTarget::ELEMENT_ARRAY_BUFFER, static_cast<unsigned>(JsonBufferViewTarget::ELEMENT_ARRAY_BUFFER)},
    });

    class JsonBufferView
    {
    public:
        unsigned buffer;
        unsigned byteLength;
        std::optional<unsigned> byteOffset;
        std::optional<unsigned> byteStride;
        std::optional<JsonBufferViewTarget> target;
    };

    NLOHMANN_DEFINE_TYPE_EXTENSION(JsonBufferView, buffer, byteLength, byteOffset, byteStride, target);

    enum class JsonAnimationChannelTargetPath
    {
        TRANSLATION,
        ROTATION,
        SCALE,
        WEIGHTS
    };

    NLOHMANN_JSON_SERIALIZE_ENUM(JsonAnimationChannelTargetPath,
                                 {
                                     {JsonAnimationChannelTargetPath::TRANSLATION, "translation"},
                                     {JsonAnimationChannelTargetPath::ROTATION,    "rotation"   },
                                     {JsonAnimationChannelTargetPath::SCALE,       "scale"      },
                                     {JsonAnimationChannelTargetPath::WEIGHTS,     "weights"    },
    });

    class JsonAnimationChannelTarget
    {
    public:
        unsigned node;
        JsonAnimationChannelTargetPath path;
    };

    NLOHMANN_DEFINE_TYPE_EXTENSION(JsonAnimationChannelTarget, node, path);

    class JsonAnimationChannel
    {
    public:
        unsigned sampler;
        JsonAnimationChannelTarget target;
    };

    NLOHMANN_DEFINE_TYPE_EXTENSION(JsonAnimationChannel, sampler, target);

    enum class JsonAnimationSamplerInterpolation
    {
        LINEAR,
        STEP,
        CUBIC_SPLINE
    };

    NLOHMANN_JSON_SERIALIZE_ENUM(JsonAnimationSamplerInterpolation,
                                 {
                                     {JsonAnimationSamplerInterpolation::LINEAR,       "LINEAR"     },
                                     {JsonAnimationSamplerInterpolation::STEP,         "STEP"       },
                                     {JsonAnimationSamplerInterpolation::CUBIC_SPLINE, "CUBICSPLINE"},
    });

    class JsonAnimationSampler
    {
    public:
        unsigned input;
        std::optional<JsonAnimationSamplerInterpolation> interpolation;
        unsigned output;
    };

    NLOHMANN_DEFINE_TYPE_EXTENSION(JsonAnimationSampler, input, interpolation, output);

    class JsonAnimation
    {
    public:
        std::vector<JsonAnimationChannel> channels;
        std::vector<JsonAnimationSampler> samplers;
        std::optional<std::string> name;
    };

    NLOHMANN_DEFINE_TYPE_EXTENSION(JsonAnimation, channels, samplers, name);

    class JsonTextureInfo
    {
    public:
        unsigned index;
    };

    NLOHMANN_DEFINE_TYPE_EXTENSION(JsonTextureInfo, index);

    class JsonPbrMetallicRoughness
    {
    public:
        std::optional<JsonTextureInfo> baseColorTexture;
        std::optional<float> metallicFactor;
    };

    NLOHMANN_DEFINE_TYPE_EXTENSION(JsonPbrMetallicRoughness, baseColorTexture, metallicFactor);

    class JsonNormalTextureInfo
    {
    public:
        unsigned index;
    };

    NLOHMANN_DEFINE_TYPE_EXTENSION(JsonNormalTextureInfo, index);

    class JsonMaterial
    {
    public:
        std::optional<std::string> name;
        std::optional<JsonPbrMetallicRoughness> pbrMetallicRoughness;
        std::optional<JsonNormalTextureInfo> normalTexture;
        std::optional<bool> doubleSided;
    };

    NLOHMANN_DEFINE_TYPE_EXTENSION(JsonMaterial, name, pbrMetallicRoughness, normalTexture, doubleSided);

    enum class JsonMeshPrimitivesMode
    {
        POINTS = 0,
        LINES = 1,
        LINE_LOOP = 2,
        LINE_STRIP = 3,
        TRIANGLES = 4,
        TRIANGLES_STRIP = 5,
        TRIANGLE_FAN = 6
    };

    NLOHMANN_JSON_SERIALIZE_ENUM(JsonMeshPrimitivesMode,
                                 {
                                     {JsonMeshPrimitivesMode::POINTS,          static_cast<unsigned>(JsonMeshPrimitivesMode::POINTS)         },
                                     {JsonMeshPrimitivesMode::LINES,           static_cast<unsigned>(JsonMeshPrimitivesMode::LINES)          },
                                     {JsonMeshPrimitivesMode::LINE_LOOP,       static_cast<unsigned>(JsonMeshPrimitivesMode::LINE_LOOP)      },
                                     {JsonMeshPrimitivesMode::LINE_STRIP,      static_cast<unsigned>(JsonMeshPrimitivesMode::LINE_STRIP)     },
                                     {JsonMeshPrimitivesMode::TRIANGLES,       static_cast<unsigned>(JsonMeshPrimitivesMode::TRIANGLES)      },
                                     {JsonMeshPrimitivesMode::TRIANGLES_STRIP, static_cast<unsigned>(JsonMeshPrimitivesMode::TRIANGLES_STRIP)},
                                     {JsonMeshPrimitivesMode::TRIANGLE_FAN,    static_cast<unsigned>(JsonMeshPrimitivesMode::TRIANGLE_FAN)   },
    });

    // This should probably be a map, but the supported models do not have arbitrary primitives anyway
    class JsonMeshPrimitivesAttributes
    {
    public:
        std::optional<unsigned> POSITION;
        std::optional<unsigned> NORMAL;
        std::optional<unsigned> COLOR_0;
        std::optional<unsigned> TEXCOORD_0;
        std::optional<unsigned> JOINTS_0;
        std::optional<unsigned> WEIGHTS_0;
    };

    NLOHMANN_DEFINE_TYPE_EXTENSION(JsonMeshPrimitivesAttributes, POSITION, NORMAL, TEXCOORD_0, JOINTS_0, WEIGHTS_0);

    class JsonMeshPrimitives
    {
    public:
        JsonMeshPrimitivesAttributes attributes;
        std::optional<unsigned> indices;
        std::optional<unsigned> material;
        std::optional<JsonMeshPrimitivesMode> mode;
    };

    NLOHMANN_DEFINE_TYPE_EXTENSION(JsonMeshPrimitives, attributes, indices, material, mode);

    class JsonMesh
    {
    public:
        std::vector<JsonMeshPrimitives> primitives;
        std::optional<std::vector<float>> weights;
    };

    NLOHMANN_DEFINE_TYPE_EXTENSION(JsonMesh, primitives, weights);

    class JsonSkin
    {
    public:
        std::optional<unsigned> inverseBindMatrices;
        std::optional<unsigned> skeleton;
        std::vector<unsigned> joints;
    };

    NLOHMANN_DEFINE_TYPE_EXTENSION(JsonSkin, inverseBindMatrices, skeleton, joints);

    class JsonScene
    {
    public:
        std::vector<unsigned> nodes;
        std::optional<std::string> name;
    };

    NLOHMANN_DEFINE_TYPE_EXTENSION(JsonScene, nodes, name);

    class JsonTexture
    {
    public:
        unsigned source;
    };

    NLOHMANN_DEFINE_TYPE_EXTENSION(JsonTexture, source);

    class JsonImage
    {
    public:
        std::optional<std::string> uri;
    };

    NLOHMANN_DEFINE_TYPE_EXTENSION(JsonImage, uri);

    class JsonRoot
    {
    public:
        std::optional<std::vector<JsonAccessor>> accessors;
        std::optional<std::vector<JsonAnimation>> animations;
        JsonAsset asset;
        std::optional<std::vector<JsonBuffer>> buffers;
        std::optional<std::vector<JsonBufferView>> bufferViews;
        std::optional<std::vector<JsonImage>> images;
        std::optional<std::vector<JsonMaterial>> materials;
        std::optional<std::vector<JsonMesh>> meshes;
        std::optional<std::vector<JsonNode>> nodes;
        std::optional<std::vector<JsonSkin>> skins;
        std::optional<unsigned> scene;
        std::optional<std::vector<JsonScene>> scenes;
        std::optional<std::vector<JsonTexture>> textures;
    };

    NLOHMANN_DEFINE_TYPE_EXTENSION(
        JsonRoot, accessors, animations, asset, buffers, bufferViews, images, materials, meshes, nodes, skins, scene, scenes, textures);
} // namespace gltf
