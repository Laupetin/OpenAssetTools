#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace techset
{
    struct CommonStreamRoutingSourceInfo
    {
        const char* name;
        const char* abbreviation;
        bool optional;
    };

    struct CommonStreamRoutingDestinationInfo
    {
        const char* name;
        const char* abbreviation;
    };

    typedef std::uint8_t CommonStreamSource;
    typedef std::uint8_t CommonStreamDestination;

    class CommonStreamRoutingInfos
    {
    public:
        CommonStreamRoutingInfos(const CommonStreamRoutingSourceInfo* sourceInfos,
                                 size_t sourceCount,
                                 const CommonStreamRoutingDestinationInfo* destinationNames,
                                 size_t destinationCount);

        [[nodiscard]] const char* GetSourceName(CommonStreamSource source) const;
        [[nodiscard]] const char* GetSourceAbbreviation(CommonStreamSource source) const;
        [[nodiscard]] bool IsSourceOptional(CommonStreamSource source) const;
        [[nodiscard]] const char* GetDestinationName(CommonStreamDestination destination) const;
        [[nodiscard]] const char* GetDestinationAbbreviation(CommonStreamDestination destination) const;

    private:
        std::vector<CommonStreamRoutingSourceInfo> m_sources;
        std::vector<CommonStreamRoutingDestinationInfo> m_destinations;
    };

    class CommonStreamRouting
    {
    public:
        CommonStreamSource m_source;
        CommonStreamDestination m_destination;
    };

    class CommonVertexDeclaration
    {
    public:
        std::vector<CommonStreamRouting> m_routing;
    };

    class CommonTechniqueShader
    {
    public:
        std::string m_name;
        const void* m_shader_bin;
        size_t m_shader_bin_size;
    };

    enum class DxVersion : std::uint8_t
    {
        DX9,
        DX11
    };

    class CommonPass
    {
    public:
        uint32_t m_sampler_flags;
        DxVersion m_dx_version;
        CommonTechniqueShader m_vertex_shader;
        CommonTechniqueShader m_pixel_shader;
        CommonVertexDeclaration m_vertex_declaration;
    };

    class CommonTechnique
    {
    public:
        std::string m_name;
        uint64_t m_flags;
        std::vector<CommonPass> m_passes;
    };
} // namespace techset
